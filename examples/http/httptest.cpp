#include <net.h>
#include <iostream>
#include <cstring>

char buff[2049] = {0};

void handle_read(int ec, int transfered, socket &sock){
	std::cout << buff << std::endl;
	bzero(buff, 2048);
	sock.async_read(buff, 2048, std::bind(handle_read, std::placeholders::_1,
				std::placeholders::_2, std::ref(sock)));
}

int main(){
	socket sock;

#if 1
	if(!sock.connect("www.baidu.com", 80)){
		std::cout << "connected failed" << std::endl;
		return -1;
	}
	std::cout << "connected successfully" << std::endl;
	
	std::string get = "GET / HTTP/1.1\r\n"
		"Host: www.baidu.com\r\n"
		"Connection: keep-alive\r\n\r\n";

	char buff[2049] = {0};
	sock.async_write(get.c_str(), get.size(), [&sock, &buff](int ec, int t){
		sock.async_read(buff, 2048, std::bind(handle_read, std::placeholders::_1,
				std::placeholders::_2, std::ref(sock)));
	});
#else
	char buff[2049] = {0};
	sock.async_connect("www.baidu.com", 80, [&sock, &buff](int ec){
		if(ec == 0){
			std::cout << "connect successfully" << std::endl;

			std::string get = "GET / HTTP/1.1\r\n"
				"Host: www.baidu.com\r\n"
				"Connection: keep-alive\r\n\r\n";

			sock.async_write(get.c_str(), get.size(), [&sock, &buff](int ec, int t){
				std::cout << "write ec = " << ec << ", t = " << t << std::endl;
				sock.async_read(buff, 2048, std::bind(handle_read, std::placeholders::_1,
					std::placeholders::_2, std::ref(sock)));
			});
		}
		else{
			std::cout << "connect failed" << std::endl;
		}
	});
#endif

	char c;
	std::cin >> c;

	return 0;
}
