#include <socket.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <cstring>
#include <errno.h>
#include <sys/epoll.h>
#include <epollor.h>

#include <iostream>

socket::socket()
	:socket_base(), 
	read_events(0), 
	write_events(0) 
{
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

bool socket::connect(const std::string &host, unsigned short port){
	if(!update_addr(host, port)){
		return false;
	}

	epollor::instance()->get_epoll()->update_request(this, EPOLLOUT|EPOLLET|EPOLLONESHOT);
	if(-1 == ::connect(sock, reinterpret_cast<struct sockaddr*>(addr.get()), 
				sizeof(struct sockaddr))){
		if(errno == EINPROGRESS){
			read_events |= ACCEPT;
			std::unique_lock<std::mutex> locker(accept_event);
			accept_cv.wait(locker);
			epollor::instance()->get_epoll()->update_request(this, events);

			return accept_status;
		}
		else{
			std::cout << errno << std::endl;
			return false;
		}
	}

	std::cout << "success" << std::endl;
	return true;;
}

int socket::sync_write(const void *buff, size_t length){
	write_events |= SYNC;
	epollor::instance()->get_epoll()->update_request(this, events|EPOLLOUT);;
	std::unique_lock<std::mutex> locker(sync_write_event);
	sync_write_cv.wait(locker);
	write_events &= ~SYNC;
	return write_some_noblock(buff, length);
}

int socket::sync_read(void *buff, size_t length){
	std::cout << "sync_read" << std::endl;
	read_events |= SYNC;
	std::unique_lock<std::mutex> locker(sync_write_event);
	sync_read_cv.wait(locker);
	read_events &= ~SYNC;
	return read_some_noblock(buff, length);
}

void socket::async_write(const void *buff, size_t length, write_callback wc){
	epollor::instance()->get_epoll()->update_request(this, events|EPOLLOUT);;
	//std::lock_guard<std::mutex> scope_locker(async_write_safe_mutex);
	wrcb = wc;
	write_buff = buff;
	write_length = length;
	write_events |= ASYNC;
}

void socket::async_read(void *buff, size_t length, read_callback rc){
	//std::lock_guard<std::mutex> scope_locker(async_read_safe_mutex);
	rdcb = rc;
	read_buff = buff;
	read_length = length;
	read_events |= ASYNC;
}

void socket::icallback(){
	if(read_events & ACCEPT){
		read_events &= ~ACCEPT;
		int error;
		socklen_t len = sizeof(error);
		if(getsockopt(sock, SOL_SOCKET, SO_ERROR, &error, &len) < 0){
			accept_status = false;
		}
		accept_status = (error == 0);
		accept_cv.notify_all();
		return;
	}
	if(read_events & SYNC){
		read_events &= ~SYNC;
		//notify sync_events
		sync_read_cv.notify_all();
	}
	if(read_events & ASYNC){
		read_events &= ~ASYNC;
		if(epollor::instance()->get_processor()->arrange(std::bind(&socket::read_proc, this))){
			std::cerr << "full" << std::endl;
		}
	}
}

void socket::ocallback(){
	if(read_events & ACCEPT){
		epollor::instance()->get_epoll()->update_request(this, events);
		read_events &= ~ACCEPT;
		int error;
		socklen_t len = sizeof(error);
		if(getsockopt(sock, SOL_SOCKET, SO_ERROR, &error, &len) < 0){
			std::cerr << "error" << std::endl;
			accept_status = false;
		}
		accept_status = (error == 0);
		accept_cv.notify_all();
		return;
	}
	if(write_events & SYNC){
		epollor::instance()->get_epoll()->update_request(this, events);
		write_events &= ~SYNC;
		//notify
		sync_write_cv.notify_all();
	}
	if(write_events & ASYNC){
		epollor::instance()->get_epoll()->update_request(this, events);
		write_events &= ~ASYNC;

		if(epollor::instance()->get_processor()->arrange(std::bind(&socket::write_proc, this))){
			std::cerr << "full" << std::endl;
		}
	}
}

void socket::ecallback(){
	if(read_events & ACCEPT){
		read_events &= ~ACCEPT;
		accept_status = false;
		int error;
		socklen_t len = sizeof(error);
		if(getsockopt(sock, SOL_SOCKET, SO_ERROR, &error, &len) < 0){
			accept_status = false;
		}
		accept_status = (error == 0);
		accept_cv.notify_all();
	}
}

int socket::read_some_noblock(void *buff, size_t len){
	int cnt = 0;
	int nread = 0;

	unsigned char *u8_buff = reinterpret_cast<unsigned char*>(buff);

	while((nread = ::read(sock, u8_buff + cnt, len - nread)) > 0){  
		cnt += nread;
	}

	if((nread == 0) || (nread == -1 && errno == EAGAIN)){
		return cnt;
	}
	return -1;
}

int socket::write_some_noblock(const void *buff, size_t len){
	const unsigned char *u8_buff 
		= reinterpret_cast<const unsigned char*>(buff);

	int cnt = 0;
	int nwrite = 0;
	int left = len;
	while(left > 0) {  
		nwrite = ::write(sock, u8_buff + cnt, left);  
		if(nwrite < left) {  
			if(nwrite == -1 && errno != EAGAIN) {  
				return -1;
			}  
		}  
		left -= nwrite;  
		cnt += nwrite;
	}
	return cnt;
}

void socket::read_proc(){
	int ret = read_some_noblock(read_buff, read_length);
	int ec = 0;
	if(rdcb != nullptr){
		read_callback rc = rdcb;
		rdcb = nullptr;
		rc(ec, ret);
	}
}

void socket::write_proc(){
	int ret = write_some_noblock(write_buff, write_length);
	int ec = 0;
	if(wrcb != nullptr){
		write_callback wc = wrcb;
		wrcb = nullptr;
		wc(ec, ret);
	}
}
