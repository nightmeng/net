#include <acceptor.h>
#include <iostream>
#include <cstring>

char buff[1024] = {0};

void handler_read(int ec, int t, std::shared_ptr<socket> sock){
	if(ec == 0){
		sock->sync_write(buff, t);
		bzero(buff, 1024);
		sock->async_read(buff, 1024, std::bind(handler_read, std::placeholders::_1, std::placeholders::_2, sock));
	}
}

void handle_accept(int ec, std::shared_ptr<socket> sock){
	if(ec == 0){
		sock->async_read(buff, 1024, std::bind(handler_read, std::placeholders::_1, std::placeholders::_2, sock));
	}
}

int main(){
	acceptor a;

	a.bind("localhost", 3215);

	a.listen();

	a.accept(handle_accept);

	char c;
	std::cin >> c;

	return 0;
}
