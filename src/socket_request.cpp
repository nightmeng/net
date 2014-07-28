#include "net/tcp/socket_request.h"
#include "net/tcp/socket.h"
#include <sys/epoll.h>

#include <iostream>

using namespace net::tcp;

socket_request::socket_request(socket_base *sock)
	:request_base(sock),
	imode(NEVER),
	omode(NEVER){
	events(EPOLLET);
}

socket_request::~socket_request(){}

int socket_request::type()const{
	return SOCKET_REQ;
}

socket::icallback socket_request::icallback(){
	return icb;
}

socket::ocallback socket_request::ocallback(){
	return ocb;
}

void socket_request::icallback(socket::icallback cb){
	icb = cb;
}

void socket_request::ocallback(socket::ocallback cb){
	ocb = cb;
}

void socket_request::ibuffer(void *buff, size_t length){
	ibuffer(std::make_tuple(buff, length));
}

void socket_request::ibuffer(const std::tuple<void*, size_t> &buff){
	ibuf = buff;
}

std::tuple<void*, size_t> socket_request::ibuffer(){
	return ibuf;
}

void socket_request::obuffer(void *buff, size_t length){
	obuffer(std::make_tuple(buff, length));
}

void socket_request::obuffer(const std::tuple<void*, size_t> &buff){
	obuf = buff;
}

std::tuple<void*, size_t> socket_request::obuffer(){
	return obuf;
}
