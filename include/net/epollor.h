#ifndef __EPOLLOR_H__
#define __EPOLLOR_H__

#include <net/epoll.h>
#include <net/factory.h>
#include <memory>

class epollor{
	public:
		~epollor();

		static std::shared_ptr<epollor> instance();
		epoll* get_epoll();
		factory *get_factory();

	private:
		epollor(const epollor&) = delete;
		epollor& operator=(const epollor&) = delete;
		epollor();

	private:
		std::shared_ptr<epoll> ep;
		std::shared_ptr<factory> p;
		static std::shared_ptr<epollor> ist;
};

#endif
