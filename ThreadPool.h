#ifndef THREADPOOL_H_
#define THREADPOOL_H_


#include<vector>
#include<deque>
#include<string>
#include<mutex>
#include<condition_variable>
#include<memory>
#include<functional>
#include"Thread.h"

class ThreadPool
{
public:
	using Task = std::function<void()>;
	//disable copy
	ThreadPool(const ThreadPool&) = delete;
	ThreadPool& operator= (const ThreadPool&) = delete;
public:
	ThreadPool(const std::string &name = "ThreadPool");
	~ThreadPool();

	void setMaxQueueSize(int maxSize);
	void setThreadInitCallback(const Task& cb);

	void start(int numThreads);
	void stop();
	void run(Task f);

	const std::string name() const;
	size_t queueSize();
private:
	bool isFull();
	void runInThread();
	Task take();
private:
	Task threadInitCallback_;
	std::vector<std::unique_ptr<Thread>> threads_;
	std::deque<Task> queue_;
	std::string name_;
	bool isRunning_;
	size_t maxQueueSize_;
	std::mutex mutex_;
	std::condition_variable_any notEmpty_;
	std::condition_variable_any notFull_;

};

#endif