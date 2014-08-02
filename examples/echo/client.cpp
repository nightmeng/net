#include <socket.h>
#include <iostream>

int main(){
	socket sock;

	if(!sock.connect("localhost", 1234)){
		std::cout << "connect failed!" << std::endl;
		return -1;
	}
	std::cout << "connect successfully" << std::endl;

	char buff[2049] = {0};
	sock.sync_read(buff, 2048);
	std::cout << "recv: " << buff << std::endl;

	sock.sync_write("hello", 5);

	return 0;
}
