#ifndef __EPOLLOR_H__
#define __EPOLLOR_H__

#include <net/epoll.h>
#include <net/processor.h>
#include <memory>

class epollor{
	public:
		static std::shared_ptr<epollor> instance();
		epoll* get_epoll();
		processor *get_processor();
		~epollor();
	private:
		epollor();
		std::shared_ptr<epoll> ep;
		std::shared_ptr<processor> p;
		static std::shared_ptr<epollor> ist;
};

#endif
