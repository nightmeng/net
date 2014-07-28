#ifndef __EPOLL_H__
#define __EPOLL_H__

#include <vector>
#include <set>
#include <memory>
#include <thread>
#include <atomic>

struct epoll_event;

namespace net{
namespace tcp{

class request_base;

class epoll{
	public:
		epoll();
		epoll(int max);
		~epoll();

		bool add_request(request_base *req);
		bool update_request(request_base *req);
		bool del_request(request_base *req);

		void poll();
		void interrupt();
	private:
		void loop();
		void process();
		bool update_req(request_base *req, int operation, bool sync = true);
		void handle_socket_request(request_base *req, int events);
		void handle_accept_request(request_base *req, int events);
	private:
		int fd;
		std::vector<struct epoll_event> events;
		std::set<int> fds;
		std::shared_ptr<std::thread> poller;
		std::atomic<bool> polling;
};
}/*tcp*/
}/*net*/

#endif
