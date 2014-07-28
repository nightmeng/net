#ifndef __EPOLLOR_H__
#define __EPOLLOR_H__

#include <memory>
#include "epoll.h"

namespace net{
namespace tcp{

class epollor{
	public:
		static std::shared_ptr<epollor> instance();
		epoll *get_epoll();
		~epollor();
	private:
		epollor();
		epollor(const epollor&) = delete;
		epollor& operator=(const epollor&) = delete;
	private:
		static std::shared_ptr<epollor> epor;
		epoll *ep;
};
}/*tcp*/
}/*net*/

#endif
