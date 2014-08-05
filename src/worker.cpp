#include <net/worker.h>

worker::worker():stop(false), onced(true){
	mutex.lock();
	stop = false;
	thread = std::shared_ptr<std::thread>(
			new std::thread(std::bind(&worker::work, this)));
}

worker::~worker(){
	interrupt();
}

void worker::interrupt(){
	if(nullptr != thread){
		stop = true;
		thread->join();
		thread = nullptr;
	}
}

void worker::work(){
	while(true){
		while(!mutex.try_lock_for(std::chrono::milliseconds(30))){
			if(stop)
				return;
		}
		std::lock_guard<std::mutex> locker(task_safe);
		if(proc != nullptr)
			proc();
		if(once){
			proc = nullptr;
			onced = true;
		}
	}
}

bool worker::task(std::function<void()> job){
	if(task_safe.try_lock()){
		once = false;
		proc = job;
		task_safe.unlock();
		return true;
	}
	return false;
}

bool worker::task_once(std::function<void()> job){
	if(onced && task_safe.try_lock()){
		once = true;
		onced = false;
		proc = job;
		task_safe.unlock();
		return true;
	}
	return false;
}

void worker::active(){
	mutex.unlock();
}
