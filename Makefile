all:lib/libnet.a examples

LIBNET_OBJS:=$(patsubst %.cpp, %.o, $(wildcard src/*.cpp))

$(shell mkdir -p lib)

lib/libnet.a:${LIBNET_OBJS}
	ar rcs $@ $^

examples:bin/echo/client bin/echo/server bin/http/httptest

bin/echo/client:examples/echo/client.o lib/libnet.a
	@mkdir -p bin/echo
	g++ -o $@ $< -L ./lib -lnet -pthread

bin/echo/server:examples/echo/server.o lib/libnet.a
	@mkdir -p bin/echo
	g++ -o $@ $< -L ./lib -lnet -pthread

bin/http/httptest:examples/http/httptest.o lib/libnet.a
	@mkdir -p bin/http
	g++ -o $@ $< -L ./lib -lnet -pthread

%.o:%.cpp
	g++ -c -o $@ $< -std=c++0x -I ./include -I ./include


.PHONY:clean
clean:
	rm src/*.o examples/echo/*.o examples/http/*.o ./lib ./bin -rf
