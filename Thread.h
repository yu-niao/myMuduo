#ifndef _THREAD_H_
#define _THREAD_H_

#include <functional>
#include <thread>
#include <memory>
#include <unistd.h>
#include <string>
#include <atomic>

#include "noncopyable.h"

class Thread : noncopyable
{
public:
    using ThreadFunc = std::function<void()>;

    explicit Thread(ThreadFunc, const std::string& name = std::string());
    ~Thread();

    void start();
    void join();
    void setDefaultName();

    bool started() const { return started_; }
    pid_t tid() const { return tid_; }
    const std::string& name() const { return name_; }
    static int numCreated() { return numCreated_; }

private:
    bool started_;
    bool joined_;
    ThreadFunc func_;
    std::shared_ptr<std::thread> thread_;
    std::string name_;
    static std::atomic_int32_t numCreated_;
    pid_t tid_;
};

#endif