#include <socket_base.h>
#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <epollor.h>
#include <sys/epoll.h>

#include <iostream>

socket_base::socket_base():events(EPOLLIN|EPOLLET){
	sock = ::socket(AF_INET, SOCK_STREAM, 0);

	epollor::instance()->get_epoll()->add_request(this, events);
	epollor::instance()->get_epoll()->poll();
}

bool socket_base::set_noblock(){
	int flags = fcntl(sock, F_GETFL, 0);
	if(flags < 0){
		return false;
	}

	if(fcntl(sock, F_SETFL, flags | O_NONBLOCK) < 0) {
		return false;
	}

	return true;
}

socket_base::~socket_base(){
	close(sock);
}

int socket_base::fd(){
	return sock;
}
