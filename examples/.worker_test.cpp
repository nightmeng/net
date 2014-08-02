#include <worker.h>
#include <iostream>

void func(){
	std::cout << "hello" << std::endl;
}

int main(){
	worker w;

	std::cout << "task" << std::endl;
	w.task(func);
	std::cout << "tasked" << std::endl;

	char c;
	while(true){
		std::cin >> c;
		w.active();
	}
}
