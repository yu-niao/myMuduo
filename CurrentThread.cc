

#include "CurrentThread.h"
#include <unistd.h>

namespace CurrentThread
{
    __thread int t_cachedTid = 0;

    void cachedTid()
    {
        if (t_cachedTid == 0)
        {
            // 通过系统调用获取当前线程的tid
            t_cachedTid = static_cast<pid_t>(::syscall(SYS_gettid));
            
        }
    }
}