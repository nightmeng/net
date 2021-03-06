#include <net/socket_base.h>
#include <net/singleton.h>
#include <net/epoll.h>
#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/epoll.h>

socket_base::socket_base():epolled(false), sock(-1){
	sock = ::socket(AF_INET, SOCK_STREAM, 0);
	register_epoll_req();
	singleton<epoll>::instance()->poll();
}

socket_base::socket_base(int fd):sock(fd), epolled(false){
	register_epoll_req();
	singleton<epoll>::instance()->poll();
}

socket_base::~socket_base(){
	unregister_epoll_req();
	close();
}

int socket_base::fd(){
	return sock;
}

bool socket_base::set_noblock(){
	if(-1 == sock){
		return false;
	}
	int flags = fcntl(sock, F_GETFL, 0);
	if(flags < 0){
		return false;
	}

	if(fcntl(sock, F_SETFL, flags | O_NONBLOCK) < 0) {
		return false;
	}

	return true;
}

void socket_base::register_epoll_req(){
	if(-1 == sock){
		return;
	}
	if(!epolled){
		singleton<epoll>::instance()->add_request(
				this, EPOLLHUP|EPOLLRDHUP|EPOLLPRI|EPOLLOUT|EPOLLIN|EPOLLET);
		epolled = true;
	}
}

void socket_base::unregister_epoll_req(){
	if(-1 == sock){
		return;
	}
	if(epolled){
		singleton<epoll>::instance()->del_request(this);
		epolled = false;
	}
}

int socket_base::open(){
	sock = ::socket(AF_INET, SOCK_STREAM, 0);
	return sock;
}

void socket_base::close(){
	unregister_epoll_req();
	::close(sock);
	sock = -1;
}
