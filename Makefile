all:out/lib/libnet.a out/examples/http

$(shell mkdir -p out/lib)
$(shell mkdir -p out/examples)

#libnet.a
OBJS:=$(patsubst %.cpp, %.o, $(wildcard src/*.cpp))

out/lib/libnet.a:${OBJS}
	ar rcs $@ $^

src/%.o:src/%.cpp
	g++ -I include -std=c++0x -c -o $@ $<

#http
HTTP_OBJS:=$(patsubst %.cpp, %.o, $(wildcard examples/http/*.cpp))
out/examples/http:${HTTP_OBJS}
	g++ -o $@ $^ -std=c++0x -L out/lib/ -lnet -pthread

examples/http/%.o:examples/http/%.cpp
	g++ -I include -c -o $@ $< -std=c++0x

.PHONY:clean dist-clean
clean:
	rm out/lib/libnet.a ${OBJS} out/examples/http ${HTTP_OBJS}

dist-clean:
	rm out -rf
