/**
 * @Author Karate Yuan
 * @Email haodong_yuan@163.com
 * @Date: 2020/5/29
 */

#pragma once
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include "LogStream.h"

class AsyncLogging;

class Logger{
public: 
    Logger(const char *filename, int line);
    ~Logger();
    LogStream &stream(){ return impl_.stream_;}
    static void setLogFileName(std::string fileName) { logFileName_ = fileName;}
    static std::string getLogFileName(){ return logFileName_;}


private:
    class Impl{
    public:
        Impl(const char *fileName, int line);
        void formatTime();
        LogStream stream_;
        int line_;
        std::string basename_;
    };
    Impl impl_;
    static std::string logFileName_;
};

#define LOG LOGGER(__FILE__, __LINE__).stream()