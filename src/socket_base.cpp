#include "net/tcp/socket_base.h"
#include <unistd.h>
#include <sys/socket.h>
#include <iostream>

using namespace net::tcp;

socket_base::socket_base():req(nullptr){
	fd = socket(AF_INET, SOCK_STREAM, 0);
}

socket_base::socket_base(int sock):req(nullptr), fd(sock){}

socket_base::~socket_base(){
	if(nullptr != req){
		delete req;
		req = nullptr;
	}
	close(fd);
}
