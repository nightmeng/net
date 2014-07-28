#include "net/tcp/acceptor.h"
#include "net/tcp/socket.h"
#include "net/tcp/epollor.h"
#include "net/tcp/accept_request.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <sys/epoll.h>
#include <arpa/inet.h>

#include <iostream>

using namespace net::tcp;

acceptor::acceptor():socket_base(){
	req = a_req = new accept_request(this);
	epollor::instance()->get_epoll()->add_request(req);
	epollor::instance()->get_epoll()->poll();
}

acceptor::~acceptor(){}

bool acceptor::bind(unsigned short port){
	struct sockaddr_in addr;
	bzero(&addr,sizeof(addr)); //把一段内存区的内容全部设置为0
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");//htonl(INADDR_ANY);
	addr.sin_port = htons(port);

	return 0 == ::bind(fd, reinterpret_cast<struct sockaddr*>(&addr),
			sizeof(struct sockaddr));
}

bool acceptor::listen(){
	return 0 == ::listen(fd, 128);
}

bool acceptor::accept(acallback callback){
	a_req->events(a_req->events()|EPOLLIN);
	a_req->callback(callback);
	epollor::instance()->get_epoll()->update_request(req);
}
/*
	int error_code = 0;
	struct sockaddr_in *client = new struct sockaddr_in();
	socklen_t len = sizeof(struct sockaddr);
	if(-1 == ::accept(fd, reinterpret_cast<struct sockaddr*>(client), &len)){	
		return false;
	}

	class socket s(fd, client);
	callback(std::ref(s), error_code);
}
*/
