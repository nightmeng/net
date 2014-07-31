#include <processor.h>
#include <iostream>

processor::processor(){
}

processor::~processor(){
}

bool processor::arrange(std::function<void()> proc){
	for(auto i = 0; i != workers.size(); ++i){
		if(workers[i]->task(proc)){
			std::cout << "1" << std::endl;
			return true;
		}
	}

	if(workers.size() < 20000){
		std::cout << "2" << std::endl;
		std::shared_ptr<worker> w(new worker());
		w->task(proc);
		workers.push_back(w);
		return true;
	}
	return false;
}
