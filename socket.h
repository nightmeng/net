#ifndef __SOCKET_H__
#define __SOCKET_H__

#include "socket_base.h"
#include <string>
#include <memory>

class socket_request;
struct sockaddr_in;

class socket : public socket_base{
	public:
		friend class epoll;
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
		socket(int sock, struct sockaddr_in *new_addr);
	private:
		socket_request *s_req;
		std::shared_ptr<struct sockaddr_in> addr;
};

#endif
