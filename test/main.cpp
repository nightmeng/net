#include "socket.h"
#include "epollor.h"
#include <iostream>

int main(){
	socket sock;

	if(!sock.connect("www.baidu.com", 80)){
		std::cout << "connect failed" << std::endl;
		return -1;
	}

	std::cout << "connected to baidu" << std::endl;

	std::string get = "GET / HTTP/1.1\r\n"
		"Host: www.baidu.com\r\n"
		"Connection: keep-alive\r\n\r\n";

	char response[2048] = {0};
#if 0
	std::cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
	std::cout << "        Test for sync             " << std::endl;
	sock.sync_write(const_cast<char*>(get.c_str()), get.size());

	sock.sync_read(response, 2048);

	std::cout << response << std::endl;
	std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;
#else
	sock.async_write(const_cast<char*>(get.c_str()), get.size(), [&response, &sock](int t, int c){
		std::cout << "async_write" << std::endl;
		sock.async_read(response, 2048, [&response](int t, int c){
			std::cout << response << std::endl;
		});	
	});
#endif

	char c;
	std::cin >> c;

	return 0;
}
