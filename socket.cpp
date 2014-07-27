#include "socket.h"
#include "epollor.h"
#include "epoll.h"
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <cstring>
#include <sys/epoll.h>
#include "socket_request.h"

#include <iostream>

socket::socket():socket_base(){
	req = s_req = new socket_request(this);
	epollor::instance()->get_epoll()->add_request(req);
	epollor::instance()->get_epoll()->poll();
}

socket::~socket(){
	epollor::instance()->get_epoll()->del_request(req);
}

bool socket::connect(const std::string &host, unsigned short port){
	struct hostent *hostent = gethostbyname(host.c_str());
	if(nullptr == hostent){
		return false;
	}
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr = *((struct in_addr *)hostent->h_addr);
	bzero(&address.sin_zero, sizeof(address.sin_zero));

	if(-1 == ::connect(fd, (struct sockaddr *)&address,
				sizeof(struct sockaddr))){
		return false;
	}

	return true;
}

int socket::sync_read(void *buff, size_t length, int timeout){
	s_req->events(s_req->events()&(~EPOLLIN));
	epollor::instance()->get_epoll()->update_request(req);
	return ::recv(fd, buff, length, timeout);
}

int socket::sync_write(void *buff, size_t length, int timeout){
	s_req->events(s_req->events()&(~EPOLLOUT));
	epollor::instance()->get_epoll()->update_request(req);
	return ::send(fd, buff, length, timeout);
}

int socket::async_read(void *buff, size_t length, socket::icallback callback){
	s_req->events(s_req->events()|EPOLLIN);
	s_req->ibuffer(buff, length);
	s_req->icallback(callback);
	epollor::instance()->get_epoll()->update_request(req);
}

int socket::async_write(void *buff, size_t length, socket::ocallback callback){
	s_req->events(s_req->events()|EPOLLOUT);
	s_req->obuffer(buff, length);
	s_req->ocallback(callback);
	epollor::instance()->get_epoll()->update_request(req);
}

int socket::disconnect(){
}