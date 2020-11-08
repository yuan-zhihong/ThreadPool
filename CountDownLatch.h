#include <mutex>
#include <condition_variable>
#ifndef COUNTDOWNLATCH_H
#define COUNTDOWNLATCH_H

class CountDownLatch {
public:
    CountDownLatch(const CountDownLatch&) = delete;
    const CountDownLatch& operator=(const CountDownLatch&) = delete;
public:
    explicit CountDownLatch(int count);
    void wait();
    void countDown();
private:
    std::mutex mutex_;
    std::condition_variable_any condition_;
    int count_;
};
#endif //MYWEBSERVER_COUNTDOWNLATCH_H