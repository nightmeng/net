#include <epoll.h>
#include <sys/epoll.h>
#include <socket_base.h>
#include <unistd.h>

#include <iostream>

epoll::epoll(){
	fd = ::epoll_create(20000);
}

epoll::~epoll(){
	interrupt();
	close(fd);
}

void epoll::add_request(socket_base *sock, int events){
	struct epoll_event ev;
	ev.events = events;
	ev.data.ptr = sock;

	epoll_ctl(fd, EPOLL_CTL_ADD, sock->fd(), &ev);
}

void epoll::update_request(socket_base *sock, int events){
	struct epoll_event ev;
	ev.events = events;
	ev.data.ptr = sock;

	epoll_ctl(fd, EPOLL_CTL_MOD, sock->fd(), &ev);
}

void epoll::del_request(socket_base *sock){
	struct epoll_event ev;
	epoll_ctl(fd, EPOLL_CTL_DEL, sock->fd(), &ev);
}

void epoll::poll(){
	if(nullptr == service)
		service = std::shared_ptr<std::thread>(new std::thread(&epoll::poll_service, this));
}

void epoll::poll_service(){
	struct epoll_event evs[20000];
	polling = true;
	while(polling){
		int cnt = ::epoll_wait(fd, evs, 20000, 30);

		for(int i = 0; i != cnt; ++i){
			struct epoll_event &ev = evs[i];

			socket_base *sock = reinterpret_cast<socket_base*>(ev.data.ptr);
			if(nullptr == sock){
				continue;
			}

			if(ev.events & EPOLLHUP){
				sock->hupevent();
			}
			else{
				if(ev.events & EPOLLERR){
					sock->eevent();
				}
				if(ev.events & EPOLLIN){
					sock->ievent();
				}
				if(ev.events & EPOLLOUT){
					sock->oevent();
				}
				if(ev.events & EPOLLRDHUP){
					sock->rdhupevent();
				}
				if(ev.events & EPOLLPRI){
					sock->prievent();
				}
			}
		}
	}
}

void epoll::interrupt(){
	if(nullptr != service){
		polling = false;
		service->join();
		service = nullptr;
	}
}
