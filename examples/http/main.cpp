#include <net.h>
#include <iostream>

int main(){
	std::cout << "initializing..." << std::endl;
	net::tcp::socket sock;

	std::cout << "connecting..." << std::endl;
	if(!sock.connect("www.baidu.com", 80)){
		std::cerr << "connect to baidu failed" << std::endl;
		return -1;
	}

	std::string get = "GET / HTTP/1.1\r\n"
		"Host: www.baidu.com\r\n"
		"Connection: keep-alive\r\n\r\n";

	std::cout << "sending..." << std::endl;
	sock.sync_write(const_cast<char*>(get.c_str()), get.size());

	std::cout << "receiving..." << std::endl;
	char buff[2048] = {0};
	sock.sync_read(buff, sizeof(buff));

	std::cout << buff << std::endl;

	return 0;
}
