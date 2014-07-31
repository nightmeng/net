#include <socket.h>
#include <iostream>

int main(int argc, char *argv[]){
	socket sock;
	unsigned short port = 6438;
	if(argc == 2)
		port = atoi(argv[1]);

	if(!sock.connect("localhost", port)){
		std::cout << "connect failed" << std::endl;
		return -1;
	}

	char buff[2048] = {0};
	sock.async_read(buff, 2048, [&sock, &buff](int t, int c){
		std::cout << "recv from server: (" << t << "," << c << ")" << buff << std::endl;

		const char *wel = "hello";
		sock.sync_write(wel, 5);
	});

	char c;
	std::cin >> c;
}
