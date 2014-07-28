#ifndef __ACCEPTOR_H__
#define __ACCEPTOR_H__

#include "socket_base.h"

namespace net{
namespace tcp{

class socket;
class accept_request;

class acceptor : public socket_base{
	public:
		typedef std::function<void (socket&, int)> acallback;
		acceptor();
		~acceptor();

		bool bind(unsigned short port);
		bool listen();
		bool accept(acallback callback);
	private:
		accept_request *a_req;
};
}/*tcp*/
}/*net*/

#endif
