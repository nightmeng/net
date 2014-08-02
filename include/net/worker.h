#ifndef __ASYNC_WORKER_H__
#define __ASYNC_WORKER_H__

#include <mutex>
#include <functional>
#include <memory>
#include <thread>

class worker{
	public:
		worker();
		~worker();
		void interrupt();
		bool task(std::function<void()> job);
		bool task_once(std::function<void()> job);
		void active();
		bool busy();
	private:
		void work();
	private:
		std::mutex task_safe;
		bool stop;
		bool once;
		bool onced;
		std::timed_mutex mutex;
		std::function<void()> proc;
		std::shared_ptr<std::thread> thread;
};

#endif
