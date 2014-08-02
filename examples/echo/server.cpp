#include <acceptor.h>
#include <iostream>

int main(){
	acceptor a;

	if(!a.bind("localhost", 1234)){
		std::cout << "bind port failed!" << std::endl;
		return -1;
	}

	if(!a.listen()){
		std::cout << "listen failed!" << std::endl;
		return -2;
	}

	a.accept([](int ec, std::shared_ptr<socket> sock){
		std::cout << "ec = " << ec << std::endl;

		const char *wel = "welcome";
		sock->sync_write(wel, 7);

		char buff[2049] = {0};
		sock->sync_read(buff, 2048);
		std::cout << buff << std::endl;
	});

	char c;
	std::cin >> c;

	return 0;
}
