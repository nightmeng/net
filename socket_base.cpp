#include <socket_base.h>
#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <epollor.h>
#include <sys/epoll.h>

#include <iostream>

socket_base::socket_base():events(EPOLLIN|EPOLLET){
	sock = ::socket(AF_INET, SOCK_STREAM, 0);
	int flags = fcntl(sock, F_GETFL, 0);
	if(flags < 0){
		//error
		std::cout << "error" << std::endl;
		return;
	}

	if(fcntl(sock, F_SETFL, flags | O_NONBLOCK) < 0) {
		//error
		std::cout << "error" << std::endl;
		return;
	}

	epollor::instance()->get_epoll()->add_request(this, events);
	epollor::instance()->get_epoll()->poll();
}

socket_base::~socket_base(){
	close(sock);
}

int socket_base::fd(){
	return sock;
}
