#ifndef __ACCEPT_REQUEST_H__
#define __ACCEPT_REQUEST_H__

#include <functional>
#include "request_base.h"
#include "acceptor.h"

namespace net{
namespace tcp{

class socket_base;

class accept_request : public request_base{
	public:
		accept_request(socket_base *sock);
		acceptor::acallback callback();
		void callback(acceptor::acallback cb);
		int type()const;
	private:
		acceptor::acallback icb;
};
}/*tcp*/
}/*net*/

#endif
