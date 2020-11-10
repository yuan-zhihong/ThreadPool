#include"ThreadPool.h"
#include<stdio.h>
#include<cassert>

using Task = std::function<void()>;
ThreadPool::ThreadPool(const std::string& name)
	:isRunning_(false),
	maxQueueSize_(0),
	mutex_(),
	notEmpty_(),
	notFull_()
{}

ThreadPool::~ThreadPool()
{
	if (isRunning_)
		stop();
}

//在start之前调用，所以没有加锁
void ThreadPool::setMaxQueueSize(int maxSize)
{
	maxQueueSize_ = maxSize;
}

void ThreadPool::setThreadInitCallback(const Task& cb)
{
	threadInitCallback_ = cb;
}

void ThreadPool::start(int numThreads)
{
	assert(threads_.empty());
	isRunning_ = true;
	threads_.reserve(numThreads);
	for (int i = 0; i < numThreads; i++){
		char id[32];
		snprintf(id, sizeof id, "%d", i + 1);
		threads_.emplace_back(std::make_unique<Thread>([this] { runInThread(); }, name_ + id));
	}
	if (numThreads == 0 && threadInitCallback_)
	{
		threadInitCallback_();
	}
}

void ThreadPool::stop()
{
	{
		std::lock_guard<std::mutex> lockGuard(mutex_);
		isRunning_ = false;
		notEmpty_.notify_all();
		notFull_.notify_all();
	}
	for (auto& thr : threads_) {
		thr->join();
	}
}

void ThreadPool::run(Task f)
{
	if (threads_.empty()) {
		f();
	}
	else{
		std::lock_guard<std::mutex> lockGuard(mutex_);
		while (isFull() && isRunning_)
		{
			notFull_.wait(mutex_);
		}
		if (!isRunning_)
			return;
		assert(!isFull());

		queue_.push_back(std::move(f));
		notEmpty_.notify_one();
	}
}

const std::string ThreadPool::name() const
{
	return name_;
}

size_t ThreadPool::queueSize()
{
	std::lock_guard<std::mutex> lockGuard(mutex_);
	return queue_.size();
}

bool ThreadPool::isFull()
{
	return maxQueueSize_ > 0 && queue_.size() >= maxQueueSize_;
}

void ThreadPool::runInThread()
{
	try
	{
		if (threadInitCallback_) {
			threadInitCallback_();
		}
		while (isRunning_){
			Task task(take());
			if (task) {
				task();
			}
		}
	}
	catch (const std::exception&ex)
	{
		fprintf(stderr, "exception caught in ThreadPool %s\n", name_.c_str());
		fprintf(stderr, "reason: %s\n", ex.what());
		abort();
	}
	catch (...)
	{
		fprintf(stderr, "unknown exception caught in ThreadPool %s\n", name_.c_str());
		throw; 
	}
}

Task ThreadPool::take()
{
	std::lock_guard<std::mutex>lockGuard(mutex_);
	while (queue_.empty() && isRunning_)
	{
		notEmpty_.wait(mutex_);
	}
	Task task;
	if (!queue_.empty()) {
		task = queue_.front();
		queue_.pop_front();
		if (maxQueueSize_ > 0) {
			notEmpty_.notify_one();
		}
	}
	return task;
}
