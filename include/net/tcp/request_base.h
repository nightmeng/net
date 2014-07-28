#ifndef __REQUEST_BASE_H__
#define __REQUEST_BASE_H__

#include <mutex>

namespace net{
namespace tcp{

class socket_base;

class request_base{
	public:
		enum req_type:int{
			SOCKET_REQ,
			ACCEPT_REQ
		};
		request_base(socket_base *sock);
		request_base(socket_base *sock, int ev);
		~request_base();

		int fd();
		int events()const;
		void events(int ev);

		int timeout()const;
		void timeout(int to);

		virtual int type()const = 0;

		std::mutex &locker();
	private:
		socket_base *sb;
		int evs;
		std::mutex lock;
		int time;
};
}/*tcp*/
}/*net*/

#endif
