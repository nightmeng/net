#ifndef __ACCEPTOR_H__
#define __ACCEPTOR_H__

#include <socket_base.h>
#include <socket.h>
#include <functional>

class acceptor : public socket_base{
	public:
		typedef std::function<void(int, std::shared_ptr<socket>)> callback;

		acceptor();
		~acceptor();

		bool bind(const std::string &ip, unsigned short port);
		bool listen();
		void accept(callback cb);
		void stop();
	private:
		virtual void icallback();
	private:
		bool status;
		callback handle_accept;
};

#endif
