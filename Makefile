test/main:
	g++ -I . -g -o test/main test/main.cpp accept_request.cpp epoll.cpp epollor.cpp request_base.cpp socket_base.cpp socket.cpp socket_request.cpp -std=c++0x -pthread

.PHONY:clean
clean:
	rm test/main
