#include <net/epollor.h>
#include <mutex>

std::shared_ptr<epollor> epollor::ist;

std::shared_ptr<epollor> epollor::instance(){
	static std::mutex mutex;
	if(ist == nullptr){
		std::lock_guard<std::mutex> locker(mutex);
		if(ist == nullptr)
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


