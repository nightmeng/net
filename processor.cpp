#include <processor.h>

processor::processor(){
}

processor::~processor(){
}

bool processor::arrange(std::function<void()> proc){
	for(auto i = 0; i != workers.size(); ++i){
		if(workers[i]->task(proc)){
			return true;
		}
	}

	if(workers.size() < 20000){
		std::shared_ptr<worker> w(new worker());
		w->task(proc);
		workers.push_back(w);
		return true;
	}
	return false;
}
