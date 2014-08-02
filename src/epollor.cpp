#include <net/epollor.h>

std::shared_ptr<epollor> epollor::ist;

std::shared_ptr<epollor> epollor::instance(){
	if(ist == nullptr){
		ist = std::shared_ptr<epollor>(new epollor());
	}
	return ist;
}

epoll* epollor::get_epoll(){
	return ep.get();
}

factory *epollor::get_factory(){
	return p.get();
}

epollor::epollor():ep(new epoll()), p(new factory()){
}

epollor::~epollor(){
}


