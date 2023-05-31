#ifndef _LOGGER_H_ 
#define _LOGGER_H_

#include <string>

#include "noncopyable.h"
#include "Timestamp.h"

#define LOG_INFO(msg, ...) \
    do \
    { \
        Logger& logger = Logger::getInstance(); \
        logger.setLevel(INFO);  \
        char buf[1024]; \
        snprintf(buf, 1024, msg, ##__VA_ARGS__);    \
        logger.log(buf); \
    }while(0);

#define LOG_ERROR(msg, ...) \
    do \
    { \
        Logger& logger = Logger::getInstance(); \
        logger.setLevel(ERROR);  \
        char buf[1024]; \
        snprintf(buf, 1024, msg, ##__VA_ARGS__);    \
        logger.log(buf); \
    }while(0);

#define LOG_FATAL(msg, ...) \
    do \
    { \
        Logger& logger = Logger::getInstance(); \
        logger.setLevel(FATAL);  \
        char buf[1024]; \
        snprintf(buf, 1024, msg, ##__VA_ARGS__);    \
        logger.log(buf); \
        exit(-1);   \
    }while(0);

#ifdef MUDEBUG
#define LOG_DEBUG(msg, ...) \
    do \
    { \
        Logger& logger = Logger::getInstance(); \
        logger.setLevel(DEBUG);  \
        char buf[1024]; \
        snprintf(buf, 1024, msg, ##__VA_ARGS__);    \
        logger.log(buf); \
    }while(0);

#else
    #define LOG_DEBUG(msg, ...)
#endif

// 日志级别
enum logLevel
{
    INFO,   // 普通信息
    ERROR,  // 错误信息
    FATAL,  // coredump
    DEBUG,  // 调试信息
};

class Logger : noncopyable
{
public:
    // 获取日志唯一实例, 饿汉单例模式
    static Logger& getInstance();
    // 设置日志级别
    void setLevel(int level);
    // 写日志
    void log(std::string msg);

private:
    Logger() {}

    int level_;
};

#endif