#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#include <functional>
#include <memory>

#include "noncopyable.h"
#include "Timestamp.h"

class EventLoop;


class Channel : noncopyable
{
public:
    using EventCallback = std::function<void()>;
    using ReadEventCallback = std::function<void(Timestamp)>;

    Channel(EventLoop* loop, int fd);
    ~Channel();

    // 处理事件
    void handleEvent(Timestamp receiveTime);

    // 设置回调函数对象
    void setReadCallback(ReadEventCallback cb) { readCallback_ = std::move(cb); }

    void setWriteCallback(EventCallback cb) { writeCallback_ = std::move(cb); }

    void setCloseCallback(EventCallback cb) { closeCallback_ = std::move(cb); }

    void setErrorCallback(EventCallback cb) { errorCallback_ = std::move(cb); }

    // 防止Channel被手动remove掉后，还在执行回调操作
    void tie(const std::shared_ptr<void>&);

    int fd() const { return fd_; }

    int events() const { return events_; }

    void set_revents(int revt) { revents_ = revt; }


    void enableReading() { events_ |= kReadEvent; update(); }
    void disableReading() { events_ &= ~kReadEvent; update(); }
    void enableWriting() { events_ |= kWriteEvent; update(); }
    void disableWriting() { events_ &= ~kWriteEvent; update(); }
    void disableAll() { events_ = kNoneEvent; update(); }

    // 返回fd当前的状态
    bool isNoneEvent() const { return events_ == kNoneEvent; }
    bool isWriting() const { return events_ & kWriteEvent; }
    bool isReading() const { return events_ & kReadEvent; }

    int index() { return index_; }
    void setIndex(int ind) { index_ = ind; }

    EventLoop* ownerLoop() { return loop_; }

    // 在 EventLoop 中删除当前的 channel
    void remove();

private:
    void update();
    void handleEventWithGuard(Timestamp receiveTime);

    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop* loop_;   // 时间循环
    const int fd_;
    int events_;    // fd感兴趣的事件
    int revents_;   // poller返回的具体发生的事件
    int index_;

    std::weak_ptr<void> tie_;
    bool tied_;

    // Channel 通道里能获知revents事件，下面函数用来负责具体回调操作
    ReadEventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback closeCallback_;
    EventCallback errorCallback_;
};

#endif