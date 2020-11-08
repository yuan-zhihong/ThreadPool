#ifndef THREAD_H
#define THREAD_H

#include <functional>
#include <memory>
#include <string>
#include <thread>
#include "CountDownLatch.h"


class Thread {
public:
    using ThreadFunc = std::function<void()>;
    explicit Thread(const ThreadFunc&, const std::string& name = std::string());
    ~Thread();
    void start();
    int join();
    bool started() const { return started_; }
    const std::string& name() const { return name_; }

private:
    void setDefaultName();
    bool started_;
    bool joined_;
    std::thread thread_;
    ThreadFunc func_;
    std::string name_;
    CountDownLatch latch_;
};
#endif