#include "net/tcp/epollor.h"
#include "net/tcp/epoll.h"

using namespace net::tcp;

std::shared_ptr<epollor> epollor::epor(nullptr);

std::shared_ptr<epollor> epollor::instance(){
	if(nullptr == epor){
		epor = std::shared_ptr<epollor>(new epollor());
	}

	return epor;
}

epoll *epollor::get_epoll(){
	return ep;
}

epollor::epollor():ep(new epoll){}

epollor::~epollor(){
	if(nullptr != ep){
		delete ep;
	}
}
