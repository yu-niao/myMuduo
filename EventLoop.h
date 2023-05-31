#ifndef _EVENTLOOP_H_
#define _EVENTLOOP_H_

#include <functional>
#include <vector>
#include <atomic>
#include <memory>
#include <mutex>

#include "CurrentThread.h"
#include "noncopyable.h"
#include "Timestamp.h"
#include "CurrentThread.h"

class Channel;
class Poller;

// 事件循环类 主要包含 Channel 和 Pooler
class EventLoop : noncopyable
{
public:
    using Functor = std::function<void()>;

    EventLoop();
    ~EventLoop();
    // 开启事件循环
    void loop();
    // 退出事件循环
    void quit();

    Timestamp poolReturnTime() const {return poolReturnTime_;}
    // 在当前loop中执行
    void runInLoop(Functor cb);
    // 把cb放入队列中，唤醒loop所在的线程，执行cb
    void queueInLoop(Functor);

    // 唤醒loop所在的线程
    void wakeup();

    // 调用 poller 的方法
    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);
    bool hasChannel(Channel* channel);
    // 判断EventLoop是否在自己的线程里面
    bool isInLoopThread() const {return threadId_ == CurrentThread::tid();}

private:
    // wake up
    void handleRead();
    // 执行回调
    void doPendingFunctors();

    using ChannelList = std::vector<Channel*>;
    
    std::atomic_bool looping_;
    std::atomic_bool quit_;
    
    const pid_t threadId_;
    Timestamp poolReturnTime_;  // poller返回发生事件的channel的时间点
    std::unique_ptr<Poller> poller_;

    int wakeupFd_;  // 当mainloop获取一个新用户的channel，通过轮询算法选择一个subloop，通过该成员选择一个subloop唤醒
    std::unique_ptr<Channel> wakeupChannel_;

    ChannelList activeChannels_;
    Channel* currentActivChannel_;

    std::atomic_bool callingPendingFunctors_; // 当前loop是否有需要执行回调的操作
    std::vector<Functor> pendingFunctors_;  // 存储loop需要执行的所有回调操作
    std::mutex mutex_;
};

#endif