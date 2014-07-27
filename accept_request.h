#ifndef __ACCEPT_REQUEST_H__
#define __ACCEPT_REQUEST_H__

#include <functional>
#include "request_base.h"

class accept_request : public request_base{
	public:
		typedef std::function<void()> accept_icallback;
		accept_icallback callback();
		void callback(accept_icallback icallback);
	private:
		accept_icallback icb;
};

#endif
