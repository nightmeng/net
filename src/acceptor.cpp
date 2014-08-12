#include <net/acceptor.h>
#include <net/singleton.h>
#include <net/scheduler.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <cstring>

#include <iostream>

acceptor::acceptor():status(false){
	set_noblock();
}

acceptor::~acceptor(){
}

void acceptor::accept(callback cb){
	acb = cb;
	status = true;
}

bool acceptor::bind(const std::string &ip, unsigned short port){
	struct hostent *hostent = gethostbyname(ip.c_str());
	if(nullptr == hostent){
		return false;
	}
	struct sockaddr_in local;
	bzero(&local, sizeof(struct sockaddr_in));
	local.sin_family = AF_INET;
	local.sin_port = htons(port);
	local.sin_addr = *(reinterpret_cast<struct in_addr*>(hostent->h_addr));
	return -1 != ::bind(sock, reinterpret_cast<struct sockaddr *>(&local), sizeof(local));
}

bool acceptor::listen(){
	return -1 != ::listen(sock, 50);
}

void acceptor::ievent(){
	if(!status)
		return;

	int fd;
	while(true){
		std::shared_ptr<struct sockaddr_in> addr(new struct sockaddr_in());
		socklen_t len = sizeof(struct sockaddr_in);

		fd = ::accept(sock, reinterpret_cast<struct sockaddr*>(addr.get()),
				&len);
		
		if(fd > 0){
			singleton<scheduler>::instance()->arrange(std::bind(acb, 0, std::shared_ptr<class socket>(
							new class socket(fd, addr))));
		}
		else{
			if(errno != EAGAIN && errno != ECONNABORTED && errno != EPROTO
					&& errno != EINTR){
				singleton<scheduler>::instance()->arrange(std::bind(acb, errno, nullptr));
				return;
			}
			else{
				return;
			}
		}
	}
}

void acceptor::oevent(){}

void acceptor::eevent(){}
