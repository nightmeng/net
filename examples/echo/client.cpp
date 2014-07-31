#include <socket.h>
#include <iostream>

int main(){
	socket sock;

	if(!sock.connect("localhost", 3215)){
		std::cout << "connect failed" << std::endl;
		return -1;
	}
	std::cout << "connect successfully" << std::endl;

	std::string content;

	while(1){
		std::cout << "[send]:";
		std::cin >> content;
		sock.async_write(content.c_str(), content.size(), [&sock](int ec, int t){
			if(ec == 0){
				char buff[1024] = {0};
				sock.sync_read(buff, 1024);
				std::cout << "[recv]:" << buff << std::endl;
			}
		});
	}

	return 0;
}
