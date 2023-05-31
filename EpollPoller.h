#ifndef _EPOLLPOLLER_H_
#define _EPOLLPOLLER_H_

#include <vector>
#include <sys/epoll.h>

#include "Poller.h"

class EpollPoller : public Poller 
{
public:
    EpollPoller(EventLoop *loop);
    ~EpollPoller() override;

    Timestamp pool(int timeoutMs, ChannelList* activeChannels) override;
    void updateChannel(Channel *channel) override;
    void removeChannel(Channel *channel) override;

private:
    static const int kInitEventListSize = 16;

    // 填写活跃连接
    void fillActiveChannels(int numEvents, ChannelList *activeChannels) const;
    // 更新channel通道
    void update(int operation, Channel *channel);

    using EventList =  std::vector<struct epoll_event>;

    int epollfd_;
    EventList events_;
};

#endif