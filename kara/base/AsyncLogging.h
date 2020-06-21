/**
 * @Author Karate Yuan
 * @Email haodong_yuan@163.com
 * @Date: 2020/5/29
 */

#pragma once

#include <functional>
#include <string>
#include <vector>

#include "CountDownLatch.h"
#include "LogStream.h"
#include "MutexLock.h"
#include "Thread.h"
#include "noncopyable.h"

class AsyncLogging : noncopyable {
 public:
  AsyncLogging(const std::string basename, int flushInterval = 2);
  ~AsyncLogging() {
    if (running_) {
      stop();
    }
  }

  void append(const char* logline, int len);

  void start() {
    running_ = true;
    thread_.start();
    latch_.wait();
  }

  void stop() {
    running_ = false;
    cond_.notify();
    thread_.join();
  }

 private:
  void threadFunc();  // 用来绑定给线程类
  typedef FixedBuffer<kLargeBuffer> Buffer;
  typedef std::vector<std::shared_ptr<Buffer>> BufferVector;
  typedef std::shared_ptr<Buffer> BufferPtr;
  const int flushInterval_;  // 刷新间隔
  bool running_;
  std::string basename_;
  Thread thread_;    // 开启了新的线程
  MutexLock mutex_;    // 保证线程安全
  Condition cond_;
  BufferPtr currentBuffer_;  // 当前缓冲
  BufferPtr nextBuffer_;     // 预备缓冲
  BufferVector buffers_;     // 待写入文件的已填满的缓冲
  CountDownLatch latch_;     // 用来干嘛
};