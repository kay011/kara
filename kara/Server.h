/**
 * @Author Karate Yuan
 * @Email haodong_yuan@163.com
 * @Date: 2020/6/12
 */

#pragma once
#include <memory>
#include "Channel.h"
#include "EventLoop.h"
#include "EventLoopThreadPool.h"
// 相当于TcpServer，简化了，没有实现单独的Acceptor类
// 直接持有 AcceptChannel_
// 主线程是一个mainReactor运行一个eventloop
// 只监听是否有新连接
// 如何实现长连接  keep-alive

class Server {
 public:
  Server(EventLoop *loop, int threadNum, int port);
  ~Server() { close(listenFd_); }
  EventLoop *getLoop() const { return loop_; }
  void start();
  void handNewConn();
  void handThisConn() { loop_->updatePoller(acceptChannel_); }

 private:
  EventLoop *loop_;  // 持有一个mianLoop线程
  int threadNum_;
  // 持有一个线程池里面的线程都是跑着EventLoop，等待着注入回调
  std::unique_ptr<EventLoopThreadPool> eventLoopThreadPool_;
  bool started_;
  // 持有一个acceptChannel
  std::shared_ptr<Channel> acceptChannel_;  // 持有一个accpetChannel
  int port_;
  int listenFd_;
  static const int MAXFDS = 100000;
};