#include <worker.h>

worker::worker():status(false), stop(false){
	mutex.lock();
	start();
}

worker::~worker(){
	interrupt();
}

bool worker::busy(){
	return status;
}

void worker::start(){
	if(nullptr == thread){
		stop = false;
		thread = std::shared_ptr<std::thread>(
				new std::thread(&worker::work, this));
	}
}

void worker::interrupt(){
	if(nullptr == thread){
		status = false;
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

		if(nullptr != proc){
			proc();
			proc = nullptr;
		}

		status = false;
	}
}

bool worker::task(std::function<void()> job){
	std::lock_guard<std::mutex> locker(single);
	if(!status){
		status = true;
		proc = job;
		mutex.unlock();

		return true;
	}

	return false;
}
