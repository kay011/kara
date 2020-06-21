/**
 * @Author Karate Yuan
 * @Email haodong_yuan@163.com
 * @Date: 2020/6/12
 */

#pragma once
#include "EventLoop.h"
#include "base/Condition.h"
#include "base/MutexLock.h"
#include "base/Thread.h"
#include "base/noncopyable.h"
// 在Thread线程中传入 EventLoop特有的ThreadFunc()
class EventLoopThread : noncopyable {
 public:
  EventLoopThread();
  ~EventLoopThread();
  EventLoop *startLoop();

 private:
  void threadFunc();
  EventLoop *loop_;
  bool exiting_;
  Thread thread_;
  MutexLock mutex_;  
  Condition cond_;
};