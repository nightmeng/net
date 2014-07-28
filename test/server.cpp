#include "acceptor.h"
#include "socket.h"

#include <iostream>

int main(){

	net::tcp::acceptor a;
	
	if(!a.bind(1234)){
		std::cout << "bind error" << std::endl;

		return -1;
	}

	if(!a.listen()){
		std::cout << "listen error" << std::endl;
		return -1;
	}


	a.accept([](net::tcp::socket &s, int c){
		std::cout << "accept" << std::endl;
		s.sync_write(const_cast<char*>("Hello"), 5);
	});


	char c;
	std::cin >> c;
}
