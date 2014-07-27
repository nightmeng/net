#ifndef __SOCKET_H__
#define __SOCKET_H__

#include "socket_base.h"
#include <string>

class socket_request;

class socket : public socket_base{
	public:
		typedef std::function<void (int, int)> icallback;
		typedef std::function<void (int, int)> ocallback;

		socket();
		~socket();

		bool connect(const std::string &host, unsigned short port);
		int disconnect();

		int sync_read(void *buff, size_t length, int timeout = 30);
		int sync_write(void *buff, size_t length, int timeout = 30);

		int async_read(void *buff, size_t length, icallback callback);
		int async_write(void *buff, size_t length, ocallback callback);
	private:
		socket_request *s_req;
};

#endif
