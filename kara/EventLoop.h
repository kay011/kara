/**
 * @Author Karate Yuan
 * @Email haodong_yuan@163.com
 * @Date: 2020/6/12
 */

#pragma once
#include "Channel.h"
#include "Epoll.h"
#include "Util.h"
#include "base/CurrentThread.h"
#include "base/Logging.h"
#include "base/Thread.h"
#include <functional>
#include <memory>
#include <vector>

#include <iostream>
using namespace std;

class EventLoop {
public:
  typedef std::function<void()> Functor;
  EventLoop();
  ~EventLoop();
  void loop();
  void quit();
  void runInLoop(Functor &&cb);
  void queueInLoop(Functor &&cb);
  bool isInLoopThread() const { return threadId_ == CurrentThread::tid(); }
  void assertInLoopThread() { assert(isInLoopThread()); }
  void shutdown(shared_ptr<Channel> channel) { shutDownWR(channel->getFd()); }
  // 封装了一层，调用Epoll对象
  void removeFromPoller(shared_ptr<Channel> channel) {
    // shutDownWR(channel->getFd());
    poller_->epoll_del(channel);
  }
  void updatePoller(shared_ptr<Channel> channel, int timeout = 0) {
    poller_->epoll_mod(channel, timeout);
  }
  void addToPoller(shared_ptr<Channel> channel, int timeout = 0) {
    poller_->epoll_add(channel, timeout);
  }

private:
  // 声明顺序 wakeupFd_ > pwakeupChannel_
  bool looping_;
  shared_ptr<Epoll> poller_;
  int wakeupFd_;   // 一个EventLoop放一个fd eventfd
  bool quit_;
  bool eventHandling_;
  mutable MutexLock mutex_;  // 互斥锁 锁什么，锁 pendingFunctors_
  std::vector<Functor> pendingFunctors_; // 每个EventLoop有自己的工作队列，这里时需要加锁的
  bool callingPendingFunctors_;
  const pid_t threadId_;
  SP_Channel pwakeupChannel_; // 每个Channel对象自始至终只属于一个EventLoop

  void wakeup();  // 唤醒什么
  void handleRead();
  void doPendingFunctors();
  void handleConn();
};
