/**
 * @Author Karate Yuan
 * @Email haodong_yuan@163.com
 * @Date: 2020/5/29
 */

#include "Logging.h"
#include <assert.h>
#include <sys/time.h>
#include <time.h>
#include <iostream>
#include "AsyncLogging.h"
#include "CurrentThread.h"
#include "Thread.h"

static pthread_once_t once_control_ = PTHREAD_ONCE_INIT;
static AsyncLogging* AsyncLogger_;

std::string Logger::logFileName_ = "./KaraServer.log";

void once_init() {
  AsyncLogger_ = new AsyncLogging(Logger::getLogFileName());
  AsyncLogger_->start();  // 启动线程
}

void output(const char* msg, int len) {
  pthread_once(&once_control_, once_init);
  AsyncLogger_->append(msg, len);
}

Logger::Impl::Impl(const char* fileName, int line)
    : stream_(), line_(line), basename_(fileName) {
  formatTime();
}

void Logger::Impl::formatTime() {
  struct timeval tv;
  time_t time;
  char str_t[26] = {0};
  gettimeofday(&tv, NULL);
  time = tv.tv_sec;
  struct tm* p_time = localtime(&time);
  strftime(str_t, 26, "%Y-%m-%d %H:%M:%S\n", p_time);
  stream_ << str_t;
}

Logger::Logger(const char* fileName, int line) : impl_(fileName, line) {}

Logger::~Logger() {
  impl_.stream_ << " -- " << impl_.basename_ << ':' << impl_.line_ << '\n';
  const LogStream::Buffer& buf(stream().buffer());
  output(buf.data(), buf.length());
}