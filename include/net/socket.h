#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <net/socket_base.h>
#include <net/worker.h>
#include <functional>
#include <list>
#include <mutex>
#include <condition_variable>
#include <string>

struct sockaddr_in;

class socket : public socket_base{
	public:
		typedef std::function<void(int, int)> icallback;
		typedef std::function<void(int, int)> ocallback;
	public:
		socket();
		socket(int fd, std::shared_ptr<struct sockaddr_in> address);
		~socket();

		bool connect(const std::string &host, unsigned short port);

		int sync_write(const void *buff, size_t length);
		int sync_read(void *buff, size_t length);

		void async_write(const void *buff, size_t length, ocallback ocb);
		void async_read(void *buff, size_t length, icallback icb);

	private:
		virtual void ievent();
		virtual void oevent();
		virtual void eevent();
		virtual void rdhupevent();
	private:
		bool update_addr(const std::string &ip, unsigned short port);
		void job(bool is_read);

		void sync_rd_action(char *buff, size_t length, int &transfered,
				std::condition_variable &cv, bool &processed);
		void sync_wr_action(const char *buff, size_t length, int &transfered, 
				std::condition_variable &cv, bool &processed);

		void async_rd_action(char *buff, size_t length, icallback icb);
		void async_wr_action(const char *buff, size_t length, ocallback ocb);

		void sync_conn_action(std::condition_variable &cv);

		int read_some(char *buff, size_t length);
		int write_some(const char *buff, size_t length);
	private:
		worker rd_worker;
		worker wr_worker;

		std::mutex rd_mutex;
		std::mutex wr_mutex;
		std::mutex conn_mutex;

		std::list<std::function<void()>> rd_request;
		std::list<std::function<void()>> wr_request;
		std::list<std::function<void()>> conn_request;

		std::shared_ptr<struct sockaddr_in> addr;

		bool rdable;
		bool wrable;

		enum status:int{
			PRECONNECT,
			CONNECTING,
			CONNECTED,
			CONNECTERR,
			DISCONNECTED
		};

		enum status connect_status;
		std::condition_variable connect_notify;
};

#endif
