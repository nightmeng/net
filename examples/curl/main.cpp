#include <socket.h>
#include <iostream>

int main(){
	socket sock;

	if(!sock.connect("www.baidu.com", 80)){
		std::cout << "connected failed" << std::endl;
		return -1;
	}
	std::cout << "connected successfully" << std::endl;
	
	std::string get = "GET / HTTP/1.1\r\n"
		"ost: www.baidu.com\r\n"
		"onnection: keep-alive\r\n\r\n";

	/*std::cout << "write cnt = " << sock.sync_write(get.c_str(), get.size()) << std::endl;

	char buff[2049] = {0};
	std::cout << "read = cnt = " << sock.sync_read(buff, 2048) << std::endl;

	std::cout << buff << std::endl;*/

	char buff[2049] = {0};
	sock.async_write(get.c_str(), get.size(), [&sock, &buff](int ec, int t){
		std::cout << "ec = " << ec << " t = " << std::endl;

		sock.async_read(buff, 2048, [&buff](int ec, int t){
			std::cout << "async_read ec = " << ec << " t = " << t << std::endl;	
			std::cout << buff << std::endl;
		});
	});

	char c;
	std::cin >> c;

	return 0;
}
