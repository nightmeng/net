#include <socket_base.h>
#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <epollor.h>
#include <sys/epoll.h>

#include <iostream>

socket_base::socket_base(){
	sock = ::socket(AF_INET, SOCK_STREAM, 0);

	epollor::instance()->get_epoll()->add_request(this, EPOLLOUT|EPOLLIN|EPOLLET);
	epollor::instance()->get_epoll()->poll();
}

socket_base::socket_base(int fd):sock(fd){
	epollor::instance()->get_epoll()->add_request(this, EPOLLOUT|EPOLLIN|EPOLLET);
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
	epollor::instance()->get_epoll()->del_request(this);
	close(sock);
	std::cout << "~socket_base" << std::endl;
}

int socket_base::fd(){
	return sock;
}
