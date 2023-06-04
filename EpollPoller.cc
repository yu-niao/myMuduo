#include <asm-generic/errno-base.h>
#include <sys/epoll.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include "EpollPoller.h"
#include "Logger.h"
#include "Channel.h"
#include "Timestamp.h"

// channel 未添加到 Poller 中
const int kNew = -1;    // channel 成员index_ = -1，表示未添加
// channel 已添加到 Poller 中
const int kAdded = 1;
// channel 从 Poller 中删除
const int kDeleted = 2;

EpollPoller::EpollPoller(EventLoop *loop)
    : Poller(loop)
    , epollfd_(::epoll_create1(EPOLL_CLOEXEC))
    , events_(kInitEventListSize)
{
    if (epollfd_ < 0)
    {
        LOG_FATAL("epoll create failed. errno is %d\n", errno);
    }
}

EpollPoller::~EpollPoller()
{
    ::close(epollfd_);
}

Timestamp EpollPoller::poll(int timeoutMs, ChannelList* activeChannels)
{
    LOG_INFO("func = %s => fd totle count: %d\n",
        __FUNCTION__,
        static_cast<int>(channels_.size()));

    int numEvents = ::epoll_wait(epollfd_, &*events_.begin(), events_.size(), timeoutMs);
    int saveErrno = errno;
    Timestamp now(Timestamp::now());

    if (numEvents > 0)
    {
        LOG_INFO("%d events happended.\n", numEvents);
        fillActiveChannels(numEvents, activeChannels);
        if (numEvents == events_.size())
        {
            events_.resize(events_.size() * 2);
        }
    }
    else if (numEvents == 0)
    {
        LOG_DEBUG("%s timeout!\n", __FUNCTION__);
    }
    else 
    {
        if (saveErrno != EINTR)
        {
            errno = saveErrno;
            LOG_ERROR("EpollPoller::poll() error!");
        }
    }

    return now;
}

void EpollPoller::fillActiveChannels(int numEvents, ChannelList *activeChannels) const
{
    for (int i = 0; i < numEvents; i++)
    {
        Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
        channel->set_revents(events_[i].events);
        // EventLoop就拿到了它的poller返回的所有发生事件的channel列表
        activeChannels->push_back(channel);
    }
}

// channel update remove => EventLoop updateChannel removeChannel => Poller updateChannel removeChannel
void EpollPoller::updateChannel(Channel *channel)
{
    const int index = channel->index();
    // 打印日志
    LOG_INFO("func = %s =>fd = %d, events = %d, index = %d\n",
        __FUNCTION__,
        channel->fd(),
        channel->events(),
        index);
    
    if (index == kNew || index == kDeleted)
    {
        int fd = channel->fd();
        if (index == kNew)
        {
            channels_[fd] = channel;
        }
        channel->setIndex(kAdded);
        update(EPOLL_CTL_ADD, channel);
    }
    else 
    {
        int fd = channel->fd();
        if (channel->isNoneEvent())
        {
            update(EPOLL_CTL_DEL, channel);
            channel->setIndex(kDeleted);
        }
        else 
        {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void EpollPoller::removeChannel(Channel *channel)
{
    const int fd = channel->fd();
    channels_.erase(fd);

    // 打印日志
    LOG_INFO("func = %s =>fd = %d",
        __FUNCTION__,
        channel->fd()
        );

    const int index = channel->index();
    if (index == kAdded)
    {
        update(EPOLL_CTL_DEL, channel);
    }
    channel->setIndex(kNew);
}

// 更新channel通道
void EpollPoller::update(int operation, Channel *channel)
{
    struct epoll_event event;
    bzero(&event, sizeof(event));
    int fd = channel->fd();
    event.events = channel->events();
    event.data.ptr = channel;
    event.data.fd = fd;

    if (::epoll_ctl(epollfd_, operation, fd, &event) < 0)
    {
        if (operation == EPOLL_CTL_DEL)
        {
            LOG_ERROR("epoll_ctl del error: %d\n", errno);
        }
        else 
        {
            LOG_FATAL("epoll_ctl add/mod errorL %d\n", errno);
        }
    }
}

