#ifndef _TIMESTAMP_H_
#define _TIMESTAMP_H_

#include <iostream>
#include <string>

class Timestamp 
{
public:
    Timestamp();
    explicit Timestamp(int64_t microSecondsSinceEpoch);

    // 获取当前时间
    static Timestamp now();

    // 将当前时间转化成 年月日, 时分秒
    std::string toString() const;
private:
    int64_t microSecondsSinceEpoch_;
};

#endif