#include <net/socket.h>
#include <net/epollor.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <cstring>
#include <unistd.h>

#include <iostream>

socket::socket():socket_base(), connect_status(PRECONNECT), wrable(false), rdable(false){
	set_noblock();
	rd_worker.task(std::bind(&socket::job, this, true));
	wr_worker.task(std::bind(&socket::job, this, false));
}

socket::socket(int fd, std::shared_ptr<struct sockaddr_in> address)
	:socket_base(fd), addr(address), wrable(false), rdable(false), connect_status(CONNECTED){
	set_noblock();
	rd_worker.task(std::bind(&socket::job, this, true));
	wr_worker.task(std::bind(&socket::job, this, false));
}

socket::~socket(){}

bool socket::update_addr(const std::string &host, unsigned short port){
	struct hostent *hostent = gethostbyname(host.c_str());
	if(nullptr == hostent){
		return false;
	}
	if(nullptr == addr){
		addr = std::shared_ptr<struct sockaddr_in>(new struct sockaddr_in());
	}
	bzero(addr.get(), sizeof(struct sockaddr_in));

	addr->sin_family = AF_INET;
	addr->sin_port = htons(port);
	addr->sin_addr = *(reinterpret_cast<struct in_addr*>(hostent->h_addr));

	return true;
}

bool socket::connect(const std::string &ip, unsigned short port){
	if(-1 == sock){
		open();
		register_epoll_req();
	}

	if(!update_addr(ip, port)){
		return false;
	}
	/*if(nullptr == addr){
		addr = std::shared_ptr<struct sockaddr_in>(new struct sockaddr_in);
	}

	addr->sin_family = AF_INET;
	addr->sin_port = htons(port);
	addr->sin_addr.s_addr = inet_addr(ip.c_str());*/

	std::mutex mutex;
	std::unique_lock<std::mutex> locker(mutex);

	connect_status = PRECONNECT;
	if(-1 == ::connect(sock, 
				reinterpret_cast<struct sockaddr*>(addr.get()), 
				sizeof(struct sockaddr))){
		if(EINPROGRESS == errno){
			connect_status = CONNECTING;
			{
				std::lock_guard<std::mutex> locker(conn_mutex);
				conn_request.push_back(
						std::bind(&socket::sync_conn_action, this, std::ref(connect_notify)));
			}
			if(rdable)
				rd_worker.active();
			else if(wrable){
				wr_worker.active();
			}

			connect_notify.wait(locker);
			return CONNECTED == connect_status;
		}
		connect_status = CONNECTERR;
		return false;
	}
	connect_status = CONNECTED;
	return true;
}

int socket::sync_write(const void *buff, size_t length){
	int transfered = 0;
	std::mutex sync_mutex;
	std::unique_lock<std::mutex> locker(sync_mutex);
	bool processed = false;
	std::condition_variable cv;
	{
		std::lock_guard<std::mutex> scope_locker(wr_mutex);
		wr_request.push_back(std::bind(&socket::sync_wr_action, this, 
					reinterpret_cast<const char*>(buff), length, std::ref(transfered),
					std::ref(cv), std::ref(processed)));
		if(wrable)
			wr_worker.active();
	}
	cv.wait(locker, [&processed]()->bool{return processed;});
	
	return transfered;
}

int socket::sync_read(void *buff, size_t length){
	int transfered = 0;
	std::mutex sync_mutex;
	std::unique_lock<std::mutex> locker(sync_mutex);
	bool processed = false;
	std::condition_variable sync_cv;
	{
		std::lock_guard<std::mutex> scope_locker(rd_mutex);
		rd_request.push_back(std::bind(&socket::sync_rd_action, this, 
					reinterpret_cast<char*>(buff), length, std::ref(transfered),
					std::ref(sync_cv), std::ref(processed)));
		if(rdable)
			rd_worker.active();
	}
	sync_cv.wait(locker, [&processed]()->bool{return processed;});

	return transfered;
}

void socket::async_write(const void *buff, size_t length, ocallback ocb){
	std::lock_guard<std::mutex> locker(rd_mutex);
	wr_request.push_back(
			std::bind(&socket::async_wr_action, this, reinterpret_cast<const char *>(buff), length, ocb));

	if(wrable)
		wr_worker.active();
}

void socket::async_read(void *buff, size_t length, icallback icb){
	std::lock_guard<std::mutex> locker(wr_mutex);
	rd_request.push_back(
			std::bind(&socket::async_rd_action, this, reinterpret_cast<char*>(buff), length, icb));

	if(rdable)
		rd_worker.active();
}

void socket::sync_rd_action(char *buff, size_t length, int &transfered, std::condition_variable &cv, bool &processed){
	transfered = read_some(buff, length);
	processed = true;
	cv.notify_all();
}

void socket::sync_wr_action(const char *buff, size_t length, int &transfered, std::condition_variable &cv, bool &processed){
	transfered = write_some(buff, length);
	processed = true;
	cv.notify_all();
}

void socket::async_rd_action(char *buff, size_t length, icallback icb){
	int transfered = read_some(buff, length);
	int ec = 0;
	epollor::instance()->get_factory()->arrange(
			std::bind(icb, ec, transfered));
}

void socket::async_wr_action(const char *buff, size_t length, ocallback ocb){
	int transfered = write_some(buff, length);
	int ec = 0;
	epollor::instance()->get_factory()->arrange(
			std::bind(ocb, ec, transfered));
}

void socket::sync_conn_action(std::condition_variable &cv){
	int error;
	socklen_t len = sizeof(error);
	if(::getsockopt(sock, SOL_SOCKET, SO_ERROR, &error, &len) < 0){
		std::cerr << "[preconnect] getsockopt failed!" << std::endl;
		return;
	}
	connect_status = (0 == error)?CONNECTED:CONNECTERR;
	connect_notify.notify_all();
}

inline int socket::read_some(char *buff, size_t length){
	int cnt = 0;
	int nread = 0;

	while(cnt < length && (nread = ::read(sock, buff, length)) > 0){
		cnt += nread;
	}

	if(nread > 0){
		return cnt;
	}
	else if(-1 == nread && EAGAIN == errno){
		rdable = false;
		return cnt;
	}
	else{
		return -1;
	}
}

inline int socket::write_some(const char *buff, size_t length){
	int cnt = 0;
	int nwrite = 0;
	while(cnt < length && (nwrite = ::write(sock, buff, length)) > 0){
		cnt += nwrite;
	}

	if(nwrite > 0){
		return cnt;
	}
	else if(-1 == nwrite && EAGAIN == nwrite){
		wrable = false;
		return cnt;
	}
	else{
		return -1;
	}
}

void socket::job(bool is_read){
	std::mutex *mutex = nullptr;
	std::list<std::function<void()>> *requests = nullptr;
	bool *status = nullptr;

	switch(connect_status){
		case CONNECTING:
			mutex = &conn_mutex;
			requests = &conn_request;
			break;
		case CONNECTED:
			if(is_read){
				mutex = &rd_mutex;
				requests = &rd_request;
				status = &rdable;
			}
			else{
				mutex = &wr_mutex;
				requests = &wr_request;
				status = &wrable;
			}
			break;
		default:
			return;
	}

	do{
		std::function<void()> callback;
		{
			std::lock_guard<std::mutex> locker(*mutex);
			if(requests->empty()){
				return;
			}
			callback = requests->front();
			requests->pop_front();
			if(nullptr == callback)
				return;
		}
		callback();
	}
	while(nullptr != status && *status);
}

void socket::ievent(){
	rdable = true;
	rd_worker.active();
}

void socket::oevent(){
	wrable = true;
	wr_worker.active();
}

void socket::rdhupevent(){
	unregister_epoll_req();
	connect_status = PRECONNECT;
	rdable = false;
	wrable = false;
	close();
}

void socket::eevent(){
	if(CONNECTING == connect_status){
		int error;
		socklen_t len = sizeof(error);
		if(::getsockopt(sock, SOL_SOCKET, SO_ERROR, &error, &len) < 0){
			std::cerr << "[preconnect] getsockopt failed!" << std::endl;
			return;
		}
		connect_status = CONNECTERR;
		connect_notify.notify_all();
	}
}
