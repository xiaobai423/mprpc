#pragma once
#include"rpcqueue.h"

// 定义宏 LOG_INFO("xxx %d %s", 20, "xxxx")
#define LOG_INFO(logmsgformat, ...) \
    do \
    {  \
        Logger &logger = Logger::GetInstance(); \
        logger.SetLogLevel(INFO); \
        char c[1024] = {0}; \
        snprintf(c, 1024, logmsgformat, ##__VA_ARGS__); \
        logger.Log(c); \
    } while(0) \

#define LOG_ERR(logmsgformat, ...) \
    do \
    {  \
        Logger &logger = Logger::GetInstance(); \
        logger.SetLogLevel(ERROR); \
        char c[1024] = {0}; \
        snprintf(c, 1024, logmsgformat, ##__VA_ARGS__); \
        logger.Log(c); \
    } while(0) \


enum loglevel{
   INFO,
   ERROR,
};

class Logger{
    public:
    static Logger&getinstance();
    //设置日志等级
    void setLogLevel(loglevel level);
    //写日志
    void log(std::string msg);
   private:
   LogQueue<std::string>m_logque;
   loglevel m_level;

   Logger();
   Logger(const Logger&) = delete;
   Logger(const Logger&&) = delete;
};