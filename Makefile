all:test/main test/server test/client

test/main:
	g++ -I . -g -o test/main test/main.cpp accept_request.cpp acceptor.cpp epoll.cpp epollor.cpp request_base.cpp socket_base.cpp socket.cpp socket_request.cpp -std=c++0x -pthread

test/server:
	g++ -I . -g -o test/server test/server.cpp accept_request.cpp acceptor.cpp epoll.cpp epollor.cpp request_base.cpp socket_base.cpp socket.cpp socket_request.cpp -std=c++0x -pthread
	
test/client:
	g++ -I . -g -o test/client test/client.cpp accept_request.cpp acceptor.cpp epoll.cpp epollor.cpp request_base.cpp socket_base.cpp socket.cpp socket_request.cpp -std=c++0x -pthread
	
.PHONY:clean
clean:
	rm test/main test/server test/client
