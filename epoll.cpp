#include "epoll.h"
#include "request_base.h"
#include "socket_request.h"
#include "accept_request.h"
#include <sys/epoll.h>
#include <iostream>
#include <thread>
#include <sys/socket.h>
#include <cstring>
#include <netinet/in.h>

epoll::epoll():events(20000){
	fd = epoll_create(20000);
}

epoll::epoll(int max):events(max){
	fd = epoll_create(max);
}

epoll::~epoll(){
	interrupt();
}

bool epoll::add_request(request_base *req){
	if(fds.find(req->fd()) != fds.end())
		return false;
	fds.insert(req->fd());

	return update_req(req, EPOLL_CTL_ADD);
}

bool epoll::update_request(request_base *req){
	if(fds.find(req->fd()) == fds.end())
		return false;

	return update_req(req, EPOLL_CTL_MOD, false);
}

bool epoll::del_request(request_base *req){
	auto itr = fds.find(req->fd());

	if(itr != fds.end()){
		if(0 == update_req(req, EPOLL_CTL_DEL)){
			fds.erase(itr);
			return true;
		}
	}

	return false;
}

bool epoll::update_req(request_base *req, int operation, bool sync){
	if(nullptr == req)
		return false;

	if(sync)
		std::unique_lock<std::mutex> lock(req->locker());
	struct epoll_event event;
	event.events = req->events();
	event.data.ptr = req;

	return 0 == epoll_ctl(fd, operation, req->fd(), &event);
}

void epoll::poll(){
	if(polling.load())
		return;
	polling.store(true);
	poller = std::shared_ptr<std::thread>(
			new std::thread(std::bind(&epoll::loop, this)));
}

void epoll::loop(){
	while(this->polling.load()){
		this->process();
	}
}

void epoll::interrupt(){
	if(nullptr != poller){
		polling.store(false);
		poller->join();
		poller = nullptr;
	}
}

inline void epoll::process(){
	int cnt = epoll_wait(fd, &events[0], events.size(), 30);
	if(-1 == cnt){
		std::cerr << "[epoll] epoll_wait failed" << std::endl;
		return;
	}

	if(0 == cnt){
		return;
	}

	for(int i = 0; i != cnt; ++i){
		struct epoll_event &ev = events[i];

		request_base *req = reinterpret_cast<request_base *>(ev.data.ptr);
		if(nullptr == req){
			std::cerr << "[epoll] fetch request failed" << std::endl;
			return;
		}

		std::unique_lock<std::mutex> lock(req->locker());
		switch(req->type()){
			case request_base::SOCKET_REQ:
				handle_socket_request(req, ev.events);
				break;
			case request_base::ACCEPT_REQ:
				handle_accept_request(req, ev.events);
				break;
		}
	}
}

void epoll::handle_socket_request(request_base *req, int events){
	socket_request *s_req = dynamic_cast<socket_request *>(req);
	if(nullptr == s_req){
		std::cerr << "[epoll] dynamic cast failed" << std::endl;
		return;
	}

	if(events & s_req->events() & EPOLLIN){
		int error_code = 0;
		void *buff; size_t length;
		std::tie(buff, length) = s_req->ibuffer();

		if(nullptr == buff || 0 == length){
			std::cout << "buff is null" << std::endl;
			return;
		}

		int transfered = ::recv(s_req->fd(), buff, length, 0); 
		if(-1 == transfered){
		}

		s_req->icallback()(transfered, error_code);

		s_req->events(s_req->events()&(~EPOLLIN));
		update_req(s_req, EPOLL_CTL_MOD, false);
	}
	else if(events & s_req->events() & EPOLLOUT){
		int error_code = 0;
		void *buff; size_t length;
		std::tie(buff, length) = s_req->obuffer();

		if(length == 0 || nullptr == buff){
			std::cout << "buff is null" << std::endl;
			return;
		}

		int transfered = ::send(s_req->fd(), buff, length, 0); 
		if(-1 == transfered){
		}

		s_req->ocallback()(transfered, error_code);

		s_req->events(s_req->events()&(~EPOLLOUT));
		update_req(s_req, EPOLL_CTL_MOD, false);
	}
}

inline void epoll::handle_accept_request(request_base *req, int events){
	accept_request *a_req = dynamic_cast<accept_request *>(req);
	if(nullptr == a_req){
		std::cerr << "[epoll] dynamic cast failed" << std::endl;
		return;
	}

	if(events & a_req->events() & EPOLLIN){
		int error_code = 0;
		struct sockaddr_in *addr = new struct sockaddr_in();
		socklen_t len = sizeof(struct sockaddr);
		int sock = ::accept(a_req->fd(), reinterpret_cast<struct sockaddr*>(addr), &len);

		if(-1 == sock){
			;
		}

		class socket s(sock, addr);
		a_req->callback()(std::ref(s), error_code);
	}
}
