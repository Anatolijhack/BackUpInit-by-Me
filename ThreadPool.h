#pragma once
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>

class ThreadPool
{
public:
	ThreadPool(int n);
	~ThreadPool();
	void enqueue(std::function<void()> task);
	void waitFinished();
private:
	std::vector<std::thread> workers;
	std::queue < std::function<void()>> tasks;
	std::mutex mtx;
	std::condition_variable cv;
	bool stop = false;
	std::atomic<int> activeTasks = 0;
	void worker();
};