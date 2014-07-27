#include "request_base.h"
#include "socket_base.h"

#include <iostream>

request_base::request_base(socket_base *sock):sb(sock), evs(0), time(-1){}

request_base::request_base(socket_base *sock, int ev):sb(sock), evs(ev), time(-1){}

request_base::~request_base(){}

int request_base::fd(){
	if(nullptr == sb)
		return -1;
	return sb->fd;
}

int request_base::events()const{
	return evs;
}

void request_base::events(int ev){
	evs = ev;
}

int request_base::timeout()const{
	return time;
}

void request_base::timeout(int to){
	time = to;
}

std::mutex &request_base::locker(){
	return lock;
}
