#ifndef __SOCKET_REQUEST_H__
#define __SOCKET_REQUEST_H__

#include "request_base.h"
#include "socket.h"
#include <tuple>
#include <functional>

class socket_request : public request_base{
	public:
		enum mode:int{
			NEVER,
			ONESHOT,
			CONTINUOUS
		};
		socket_request(socket_base *sock);
		~socket_request();

		virtual int type() const;

		socket::icallback icallback();
		socket::ocallback ocallback();

		void icallback(socket::icallback cb);
		void ocallback(socket::ocallback cb);

		void ibuffer(void *buff, size_t length);
		void ibuffer(const std::tuple<void *, size_t>& buff);

		std::tuple<void*, size_t> ibuffer();

		void obuffer(void *buff, size_t length);
		void obuffer(const std::tuple<void *, size_t>& buff);

		std::tuple<void*, size_t> obuffer();
	private:
		socket_base *sb;
		socket::icallback icb;
		socket::ocallback ocb;
		std::tuple<void *, size_t> ibuf;
		std::tuple<void *, size_t> obuf;
		mode imode;
		mode omode;
};

#endif
