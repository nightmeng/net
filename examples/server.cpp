#include <acceptor.h>
#include <iostream>

int main(int argc, char *argv[]){
	acceptor a;

	unsigned short port = 6438;
	if(argc == 2)
		port = atoi(argv[1]);
	a.bind("localhost", port);

	a.listen();

	char wel[] = "welcome";
	char buff[2048] = {0};
	a.accept([&wel](int ec, std::shared_ptr<socket> sock){
		std::cout << "accept: fd = " << sock->fd() << " error code = " << ec << std::endl;		
		sock->async_write(wel, 7, [sock](int t, int c){
			char buff[2048] = {0};
			sock->sync_read(buff, 2048);
			std::cout << "recv from client: " << buff << std::endl; 
		});
	});

	char c;
	std::cin >> c;
}
