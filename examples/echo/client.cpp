#include <net.h>
#include <iostream>
#include <string>

int main(){
	socket sock;

	if(!sock.connect("localhost", 1234)){
		std::cout << "connect failed!" << std::endl;
		return -1;
	}
	std::cout << "connect successfully" << std::endl;

	std::string msg;
	while(true){
		std::cout << "[send]: ";
		std::cin >> msg;

		sock.sync_write(msg.c_str(), msg.size());

		char buff[2049] = {0};
		sock.sync_read(buff, 2048);

		std::cout << "[recv]: " << buff << std::endl;
	}

	return 0;
}
