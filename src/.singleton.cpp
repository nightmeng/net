#include <net/singleton.h>

std::shared_ptr<singleton> singleton::ist;

std::shared_ptr<singleton> singleton::instance(){
	if(ist == nullptr){
		ist = std::shared_ptr<singleton>(new singleton());
	}
	return ist;
}

epoll* singleton::get_epoll(){
	return ep.get();
}

scheduler *singleton::get_scheduler(){
	return p.get();
}

singleton::singleton():ep(new epoll()), p(new scheduler()){
}

singleton::~singleton(){
}


