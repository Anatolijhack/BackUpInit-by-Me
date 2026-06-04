#include "ThreadPool.h"

ThreadPool::ThreadPool(int n)
{
	for (int i = 0; i < n; i++)
	{
		workers.emplace_back(&ThreadPool::worker, this);
	}
}
ThreadPool::~ThreadPool()
{
	{
		std::lock_guard<std::mutex> lock(mtx);
		stop = true;
	}
	cv.notify_all();
	for (auto& t : workers)
	{
		t.join();
	}
}
void ThreadPool::enqueue(std::function<void()> task)
{
	activeTasks++;

	{
		std::unique_lock<std::mutex> lock(mtx);
		tasks.push([this, task]()
			{
				task();
				activeTasks--;
			});
	}

	cv.notify_one();
}
void ThreadPool::worker()
{
	while (true)
	{
		std::function<void()> task;
		{

			std::unique_lock<std::mutex> lock(mtx);
			cv.wait(lock, [this] {return stop || !tasks.empty(); });

			if (stop && tasks.empty())
			{
				return;
			}
			task = std::move(tasks.front());
			tasks.pop();

		}
		task();
	}
}
void ThreadPool::waitFinished()
{
	while (activeTasks.load() > 0)
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
}