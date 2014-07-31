#include <socket.h>
#include <iostream>
#include <cstring>
#include <thread>
#include <epollor.h>
#include <epoll.h>
#include <fstream>

void handler_read(int c, int t, char* buff, socket& sock){
	std::cout << "read_handler" << std::endl;
	std::cout.write(buff, t);
	bzero(buff, 1024*1024);
	sock.async_read(buff, 1024*1024, std::bind(handler_read, std::placeholders::_1, std::placeholders::_2, buff, std::ref(sock)));
}

int main(){
	socket sock;

#if 1
	if(!sock.connect("www.baidu.com", 80)){
#else
	if(!sock.connect("localhost", 10000)){
#endif
		std::cout << "connect to baidu failed" << std::endl;
		return -1;
	}

	std::cout << "connect to baidu successfully" << std::endl;

	const char *str = "GET / HTTP/1.1\r\nHost: www.baidu.com\r\nConnection: keep-alive\r\n\r\n";

#if 0
	sock.sync_write(const_cast<char*>(str), strlen(str));

	std::cout << "write" << std::endl;

	char buff[2049] = {0};
	int len = sock.sync_read(buff, 2048);
	//std::cout << buff << std::endl;
	std::cout << len << std::endl;

	bzero(buff, 2049);
	sock.sync_read(buff, 2048);
	std::cout << len << std::endl;
	//std::cout << buff << std::endl;
#else
	char buff[1024*1024+1] = {0};
	sock.async_write(const_cast<char*>(str), strlen(str), [&buff, &sock](int ec, int t){
		std::cout << "write_handler" << ec <<  " " << t << std::endl;
#if 1
		sock.async_read(buff, 1024*1024, std::bind(handler_read, std::placeholders::_1, std::placeholders::_2, buff, std::ref(sock)));
#else
		sock.sync_read(buff, 1024*1024);
		std::cout << buff << std::endl;
#endif
	});


#if 0
	while(true){
		int ret = sock.sync_read(buff, 1024*1024);
		if(ret == -1){
			std::cout << "error" << std::endl;
			break;
		}
		buff[ret] = 0;
		std::cout << buff << std::endl;
	}
#else
	//sock.async_read(buff, 1024*1024, std::bind(handler_read, std::placeholders::_1, std::placeholders::_2, buff, std::ref(sock)));
#endif

	char c;
	std::cin >> c;
#endif

	return 0;
}
