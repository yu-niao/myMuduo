#ifndef _POLLER_H_
#define _POLLER_H_

#include <vector>
#include <unordered_map>

#include "Timestamp.h"
#include "noncopyable.h"

class EventLoop;
class Channel;

// muduo 库中多路事件分发器的核心IO服用模块
class Poller : noncopyable
{
public:
    using ChannelList = std::vector<Channel*>;

    Poller(EventLoop* loop);
    virtual ~Poller();

    // 给所有IO复用保留统一的接口
    virtual Timestamp poll(int timeoutMs, ChannelList* activeChannels) = 0;
    virtual void updateChannel(Channel* channel) = 0;
    virtual void removeChannel(Channel* channel) = 0;

    // 判断参数Channel是否在当前Poller当中
    virtual bool hasChannel(Channel* channel) const;

    // EventLoop可以通过该接口获取默认的IO复用的具体实现
    static Poller* newDefaultPoller(EventLoop* loop);

protected:
    // map 的 key 表示 Channel->fd_，value：fd所属的Channel
    using ChannelMap = std::unordered_map<int, Channel*>;
    ChannelMap channels_;

private:
    // poller 所属的 EventLoop
    EventLoop* ownerLoop_;
};

#endif