#include "socket.h"
#include <iostream>

int main(){
	net::tcp::socket sock;
	if(sock.connect("127.0.0.1", 1234)){
		std::cout << "connect successfully" << std::endl;
	}
	else{
		std::cout << "connected failed" << std::endl;
	}

	char buff[6] = {0};
	sock.sync_read(buff, 6);

	std::cout << buff << std::endl;

	return 0;
}
