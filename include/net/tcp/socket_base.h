#ifndef __SOCKET_BASE_H__
#define __SOCKET_BASE_H__

#include <functional>
#include "request_base.h"

namespace net{
namespace tcp{
class socket_base{
	public:
		friend class request_base;
		typedef std::function<void()> icallback;
		typedef std::function<void()> ocallback;

		socket_base();
		socket_base(int sock);
		~socket_base();

	protected:
		int fd;
		request_base *req;
};
}/*tcp*/
}/*net*/

#endif
