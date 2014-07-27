#ifndef __ACCEPTOR_H__
#define __ACCEPTOR_H__

#include "socket_base.h"

class acceptor : public socket_base{
	public:
		typedef std::function<void (int, int)> acallback;
		acceptor();
		~acceptor();

		bool bind(unsigned short port);
		bool accept(acallback callback);
	private:
		acceptor_request *req;
};

#endif
