#include <net/factory.h>

factory::factory(){
}

factory::~factory(){
}

bool factory::arrange(std::function<void()> proc){
	for(auto i = 0; i != workers.size(); ++i){
		if(workers[i]->task_once(proc)){
			workers[i]->active();
			return true;
		}
	}

	if(workers.size() < 20000){
		std::shared_ptr<worker> w(new worker());
		w->task_once(proc);
		w->active();
		workers.push_back(w);
		return true;
	}
	return false;
}
