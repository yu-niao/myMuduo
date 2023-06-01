

#include "EventLoopThread.h"
#include "EventLoopThreadPool.h"
#include <memory>

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop, const std::string & name)
    : baseLoop_(baseLoop)
    , name_(name)
    , started_(false)
    , numThreads_(0)
    , next_(0)
{}

EventLoopThreadPool::~EventLoopThreadPool()
{}

void EventLoopThreadPool::start(const ThreadInitCallback& cb)
{
    started_ = true;

    for (int i = 0; i < numThreads_; i++)
    {
        char buf[name_.size() + 32];
        snprintf(buf, sizeof(buf), "%s%d", name_.c_str(), i);
        EventLoopThread* t = new EventLoopThread(cb, buf);
        // auto k = std::make_unique<EventLoopThread>(cb, buf);
        threads_.emplace_back(std::unique_ptr<EventLoopThread>(t));
        loops_.push_back(t->startLoop());
    }


    // 整个服务器只有一个线程，运行着baseloop
    if (numThreads_ == 0)
    {
        cb(baseLoop_);
    }
}

// 如果工作在多线程，baseloop会以轮询的方式分配channel给subloop
EventLoop* EventLoopThreadPool::getNextLoop()
{

}

std::vector<EventLoop*> EventLoopThreadPool::getAllLoops()
{

}
