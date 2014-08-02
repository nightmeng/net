#include <acceptor.h>
#include <iostream>

char buff[2049] = {0};

void handle_read(int ec, int t, std::shared_ptr<socket> sock){
	if(ec == 0){
		sock->sync_write(buff, t);
		sock->async_read(buff, 2048, std::bind(handle_read, std::placeholders::_1,
					std::placeholders::_2, sock));
	}
	else{
		std::cerr << "async read error, errno = " << ec << std::endl;
	}
}

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
		std::cout << "accept a connection" << std::endl;
		sock->async_read(buff, 2048, std::bind(handle_read, std::placeholders::_1,
					std::placeholders::_2, sock));
	});

	char c;
	std::cin >> c;

	return 0;
}
