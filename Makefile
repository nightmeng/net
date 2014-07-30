OBJS:=$(patsubst %.cpp, %.o, $(wildcard *.cpp))
main:$(OBJS)
	g++ -o $@ $^ -std=c++0x -pthread

%.o:%.cpp
	g++ -c -o $@ $< -I . -std=c++0x

.PHONY:clean
clean:
	rm *.o main
