#include "acceptor.h"
#include <sys/socket.h>

acceptor::acceptor():socket_base(this){}

acceptor::~acceptor(){}

bool acceptor::bind(unsigned short port){
	struct sockaddr_in addr;
	bzero(&addr,sizeof(addr)); //把一段内存区的内容全部设置为0
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htons(INADDR_ANY);
	addr.sin_port = htons(port);
	return ::bind(fd, reinterpret_cast<struct sockaddr*>(&addr),
			sizeof(struct sockaddr));
}

bool acceptor::sync_accept(acallback callback){
	struct sockaddr_in client;
	if(-1 == ::accept(fd, reinterpret_cast<struct sockaddr*>(&client),
				sizeof(struct sockaddr))){
		return false;
	}
}
