/**
 * @Author Karate Yuan
 * @Email haodong_yuan@163.com
 * @Date: 2020/6/12
 */

#include "EventLoopThread.h"
#include <functional>

EventLoopThread::EventLoopThread()
    : loop_(NULL),
      exiting_(false),
      thread_(bind(&EventLoopThread::threadFunc, this), "EventLoopThread"),
      mutex_(),
      cond_(mutex_) {}

EventLoopThread::~EventLoopThread() {
  exiting_ = true;
  if (loop_ != NULL) {
    loop_->quit();
    thread_.join();
  }
}

EventLoop *EventLoopThread::startLoop() {
  assert(!thread_.started());
  thread_.start();
  {
    MutexLockGuard lock(mutex_);
    // 一直等到threadFun在Thread里真正跑起来
    while (loop_ == NULL) cond_.wait();
  }
  return loop_;
}
//  线程注册的回调就是跑一个loop
void EventLoopThread::threadFunc() {
  EventLoop loop;

  {
    MutexLockGuard lock(mutex_);
    loop_ = &loop;
    cond_.notify();
  }

  loop.loop();
  // assert(exiting_);
  loop_ = NULL;
}