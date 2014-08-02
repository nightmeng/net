LIBNET_OBJS:=$(patsubst %.cpp, %.o, $(wildcard src/*.cpp))

$(shell mkdir -p lib)

lib/libnet.a:${LIBNET_OBJS}
	ar rcs $@ $^

%.o:%.cpp
	g++ -c -o $@ $< -std=c++0x -I include


.PHONY:clean
clean:
	rm src/*.o ./lib ./bin -rf
