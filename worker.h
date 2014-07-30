#ifndef __ASYNC_WORKER_H__
#define __ASYNC_WORKER_H__

#include <mutex>
#include <functional>
#include <atomic>
#include <memory>
#include <thread>

class worker{
	public:
		worker();
		~worker();
		void interrupt();
		void start();
		bool busy();
		bool task(std::function<void()> job);
	private:
		void work();
	private:
		std::mutex single;
		bool status;
		bool stop;
		std::timed_mutex mutex;
		std::function<void()> proc;
		std::shared_ptr<std::thread> thread;
};

#endif
