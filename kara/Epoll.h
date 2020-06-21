/**
 * @Author Karate Yuan
 * @Email haodong_yuan@163.com
 * @Date: 2020/6/12
 */

#pragma once
#include <sys/epoll.h>
#include <memory>
#include <unordered_map>
#include <vector>
#include "Channel.h"
#include "HttpData.h"
#include "Timer.h"
// EventLoop持有Epoll对象
// poll中调用了getEventsRequest()
// getEventsRequest
class Epoll {
 public:
  Epoll();
  ~Epoll();
  // 这里要明白 SP_Channel持有fd和对应的events
  void epoll_add(SP_Channel request, int timeout);  // 封装了epoll_ctl
  void epoll_mod(SP_Channel request, int timeout);
  void epoll_del(SP_Channel request);
  // 封装了epoll_wait,epoll_wait的返回值传入getEventsRequest
  std::vector<std::shared_ptr<Channel>> poll();

  int getEpollFd() { return epollFd_; }
  void handleExpired();  // 处理超时

 private:
  std::vector<std::shared_ptr<Channel>> getEventsRequest(
      int events_num);  // private 被poll调用
  void add_timer(std::shared_ptr<Channel> request_data, int timeout);

 private:
  static const int MAXFDS = 100000;
  int epollFd_;
  std::vector<epoll_event> events_;  // 存放的是活跃的事件
  std::shared_ptr<Channel> fd2chan_[MAXFDS];
  std::shared_ptr<HttpData> fd2http_[MAXFDS];
  TimerManager timerManager_; // 定时器堆
};