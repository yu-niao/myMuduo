#include <iostream>

#include "Logger.h"
#include "Timestamp.h"

// 获取日志唯一实例
Logger& Logger::getInstance()
{
    static Logger logger;
    return logger;
}

// 设置日志级别
void Logger::setLevel(int level)
{
    level_ = level;
}

// 写日志
// [日志级别] + 时间 + msg
void Logger::log(std::string msg)
{
    Timestamp ts;

    switch (level_)
    {
    case INFO:
        std::cout << "[INFO]";
        break;
    case ERROR:
        std::cout << "[ERROR]";
        break;
    case FATAL:
        std::cout << "[FATAL]";
        break;
    case DEBUG:
        std::cout << "[DEBUG]";
        break;
    default:
        break;    
    }

    std::cout << ts.now().toString() << " -> " << msg << std::endl;
}

#if 0
int main()
{
    LOG_INFO("打印普通信息%02d", 4);

    LOG_ERROR("打印错误信息%04d", 6);

    return 0;
}
#endif