#include <mutex>
#include <sys/eventfd.h>
#include <unistd.h>
#include <errno.h>

#include "Channel.h"
#include "EventLoop.h"
#include "Logger.h"
#include "Poller.h"

// 防止一个线程创建多个EventLoop对象
__thread EventLoop* t_loopInThisThread = nullptr;

// 定义默认的Poller IO复用接口的超时时间
const int kPollTimeMs = 1000000;

// 创建wakeupfd，用notify唤醒subReactor处理新来的channel
int createEventfd()
{
    int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (evtfd < 0)
    {
        LOG_FATAL("eventfd error: %d.\n", errno);
    }

    return evtfd;   
}

EventLoop::EventLoop()
    : looping_(false)
    , quit_(false)
    , callingPendingFunctors_(false)
    , threadId_(CurrentThread::tid())
    , poller_(Poller::newDefaultPoller(this))
    , wakeupFd_(createEventfd())
    , wakeupChannel_(new Channel(this, wakeupFd_))
    , currentActivChannel_(nullptr)
{
    LOG_DEBUG("EventLoop create %p in thread %d. \n", this, threadId_);
    if (t_loopInThisThread)
    {
        LOG_FATAL("Another EventLoop %p exists in this thread %d.\n",
        t_loopInThisThread,
        threadId_)
    }
    else 
    {
        t_loopInThisThread = this;
    }

    // 设置wakeupfd的事件类型及其发生事件后的回调操作
    wakeupChannel_->setReadCallback(std::bind(&EventLoop::handleRead, this));
    // 每一个evnetloop都将监听wakeupchannel的EPOLLIN读时间
    wakeupChannel_->enableReading();
}

EventLoop::~EventLoop()
{
    wakeupChannel_->disableAll();
    wakeupChannel_->remove();

    ::close(wakeupFd_);
    t_loopInThisThread = nullptr;
}

void EventLoop::handleRead()
{
  uint64_t one = 1;
  ssize_t n = read(wakeupFd_, &one, sizeof(one));
  if (n != sizeof(one))
  {
    LOG_ERROR("EventLoop::handleRead() reads %ld bytes instead of 8.\n", n);
  }
}

// 开启事件循环
void EventLoop::loop()
{
    looping_ = true;
    quit_ = false;

    LOG_INFO("EventLoop %p start looping!\n", this);

    while(!quit_)
    {
        activeChannels_.clear();
        poolReturnTime_ = poller_->poll(kPollTimeMs, &activeChannels_);
        for (auto& channel : activeChannels_)
        {
            // Poller监听哪些channel发生了事件，然后上报给EventLoop，通知channel处理相应的事件
            channel->handleEvent(poolReturnTime_);
        }
        // 执行当前EventLoop事件循环需要处理的回调操作
        doPendingFunctors();
    }

    LOG_INFO("EventLoop %p stop looping!\n", this);
    looping_ = false;
}

// 退出事件循环
void EventLoop::quit()
{
    quit_ = true;
    if (!isInLoopThread())
    {
        wakeup();
    }
}

// 在当前loop中执行
void EventLoop::runInLoop(Functor cb)
{
    if (isInLoopThread())
    {
        cb();
    }
    else 
    {
        queueInLoop(cb);
    }
}

// 把cb放入队列中，唤醒loop所在的线程，执行cb
void EventLoop::queueInLoop(Functor cb)
{
    {
        std::unique_lock<std::mutex> lock(mutex_);
        pendingFunctors_.emplace_back(cb);
    }
    // 唤醒相应的，需要执行上面回调操作的loop线程
    if (!isInLoopThread() || callingPendingFunctors_)
    {
        wakeup();
    }
}

// 唤醒loop所在的线程
void EventLoop::wakeup()
{
    uint64_t one = 1;
    ssize_t n = write(wakeupFd_, &one, sizeof(one));
    if (n != sizeof(one))
    {
        LOG_FATAL("EventLoop::wakeup() warite %ld bytes.\n", n);
    } 
}

// 调用 poller 的方法
void EventLoop::updateChannel(Channel *channel)
{
    poller_->updateChannel(channel);
}
void EventLoop::removeChannel(Channel *channel)
{
    poller_->removeChannel(channel);
}
bool EventLoop::hasChannel(Channel *channel)
{
    return poller_->hasChannel(channel);
}

// 执行回调
void EventLoop::doPendingFunctors()
{
    std::vector<Functor> functors;
    callingPendingFunctors_ = true;

    {
        std::unique_lock<std::mutex> lock(mutex_);
        functors.swap(pendingFunctors_);
    }

    for (const Functor& functor : functors)
    {
        functor();
    }
    
    callingPendingFunctors_ = false;
}