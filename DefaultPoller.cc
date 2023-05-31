#include <stdlib.h>

#include "EpollPoller.h"
#include "Poller.h"

Poller* Poller::newDefaultPoller(EventLoop* loop)
{
    if (::getenv("MUDUO_USE_POLL"))
    {
        return nullptr; // 生成poll示例
    }
    else 
    {
        return new EpollPoller(loop);
    }
}