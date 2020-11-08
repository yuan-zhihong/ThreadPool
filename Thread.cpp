#include "Thread.h"
#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include <memory>





Thread::Thread(const ThreadFunc& func, const std::string& n)
    : started_(false),
    joined_(false),
    thread_(),
    func_(func),
    name_(n),
    latch_(1) {
    setDefaultName();
}

Thread::~Thread() {
    if (started_ && !joined_)
        thread_.detach();
}

void Thread::setDefaultName() {
    if (name_.empty()) {
        char buf[32];
        snprintf(buf, sizeof buf, "Thread");
        name_ = buf;
    }
}

void Thread::start() {
    assert(!started_);
    started_ = true;
    thread_ = std::thread(func_);
    latch_.wait();
}

int Thread::join() {
    assert(started_);
    assert(!joined_);
    joined_ = true;
    if (thread_.joinable()) {
        thread_.join();
    }
    else
        return -1;
}