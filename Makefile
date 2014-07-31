OBJS:=$(patsubst %.cpp, %.o, $(wildcard *.cpp))

all: examples/main examples/client examples/server

examples/main:examples/main.o libnet.a
	g++ -g -o $@ $< -L . -lnet -pthread -I .

examples/client:examples/client.o libnet.a
	g++ -g -o $@ $< -L . -lnet -pthread -I .

examples/server:examples/server.o libnet.a
	g++ -g -o $@ $< -L . -lnet -pthread -I .

libnet.a:$(OBJS)
	ar rcs libnet.a $^


%.o:%.cpp
	g++ -g -c -o $@ $< -I . -std=c++0x

.PHONY:clean
clean:
	rm *.o libnet.a examples/main examples/server examples/client examples/*.o
