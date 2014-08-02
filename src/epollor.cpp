#include <epollor.h>

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

processor *epollor::get_processor(){
	return p.get();
}

epollor::epollor():ep(new epoll()), p(new processor()){
}

epollor::~epollor(){
}


