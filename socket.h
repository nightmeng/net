#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <socket_base.h>
#include <string>
#include <memory>
#include <functional>
#include <mutex>
#include <condition_variable>

struct sockaddr_in;

class socket : public socket_base{
	public:
		typedef std::function<void(int, int)> write_callback;
		typedef std::function<void(int, int)> read_callback;
	public:
		explicit socket();
		explicit socket(int fd, std::shared_ptr<struct sockaddr_in> addr);
		~socket();

		bool connect(const std::string &host, unsigned short port);
		void disconnect();

		int sync_read(void *buff, size_t length);
		int sync_write(const void *buff, size_t length);

		void async_read(void *buff, size_t length, write_callback wc);
		void async_write(const void *buff, size_t length, read_callback rc);
	private:
		bool update_addr(const std::string &host, unsigned short port);
		void work();
		int read_some_noblock(void *buff, size_t len);
		int write_some_noblock(const void *buff, size_t len);

		void read_proc();
		void write_proc();
	private:
		virtual void icallback(); //override/final
		virtual void ocallback(); //override/final
		virtual void ecallback();
	private:
		std::shared_ptr<struct sockaddr_in> addr;
	private:
		enum method:int{
			NONE = 0,
			SYNC = 1,
			ASYNC = 2,
			ACCEPT = 4,
			AAAA = 8
		};
		int read_events;
		int write_events;
	private:
		write_callback wrcb;
		const void *write_buff;
		size_t write_length;

		read_callback rdcb;
		void *read_buff;
		size_t read_length;

		std::mutex sync_read_event;
		std::mutex sync_write_event;
		std::mutex accept_event;

		std::condition_variable sync_read_cv;
		std::condition_variable sync_write_cv;
		std::condition_variable accept_cv;

		std::mutex async_read_safe_mutex;
		std::mutex async_write_safe_mutex;

		bool accept_status;
};

#endif
