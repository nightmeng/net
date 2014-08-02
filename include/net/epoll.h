#ifndef __EPOLL_H__
#define __EPOLL_H__

#include <thread>
#include <memory>
class socket_base;

class epoll{
	public:
		epoll();
		~epoll();
		void add_request(socket_base *sock, int events);
		void update_request(socket_base *sock, int events);
		void del_request(socket_base *sock);
		void poll();
		void interrupt();
	private:
		void poll_service();
	private:
		bool polling;
		int fd;
		std::shared_ptr<std::thread> service;
};

#endif
