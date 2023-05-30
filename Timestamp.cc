#include <string>

#include "Timestamp.h"

Timestamp::Timestamp()
    : microSecondsSinceEpoch_(0)
    {}

Timestamp::Timestamp(int64_t microSecondsSinceEpoch)
    : microSecondsSinceEpoch_(microSecondsSinceEpoch)
    {}

// 获取当前时间
Timestamp Timestamp::now()
{
    // microSecondsSinceEpoch_ = time(nullptr);
    return Timestamp(time(nullptr));
}

// 将当前时间转化成 年月日, 时分秒
std::string Timestamp::toString() const
{
    char buf[128] = {0};
    struct tm* localTime = localtime(&microSecondsSinceEpoch_);

    snprintf(buf, 128, "%4d/%02d/%02d %02d:%02d:%02d",
             localTime->tm_year + 1900,
             localTime->tm_mon + 1,
             localTime->tm_mday,
             localTime->tm_hour,
             localTime->tm_min,
             localTime->tm_sec);
    return buf;
}

#if 0
int main()
{
    Timestamp ts;
    std::cout << ts.now().toString() << std::endl;

    return 0;
}

#endif