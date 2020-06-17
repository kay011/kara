/**
 * @Author Karate Yuan
 * @Email haodong_yuan@163.com
 * @Date: 2020/6/12
 */

#include "Channel.h"
#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <queue>
#include "Epoll.h"
#include "EventLoop.h"
#include "Util.h"

using namespace std;

Channel::Channel(EventLoop *loop)
    : loop_(loop), fd_(0), events_(0), lastEvents_(0) {}

Channel::Channel(EventLoop *loop, int fd)
    : loop_(loop), fd_(fd), events_(0), lastEvents_(0) {}

int Channel::getFd() { return fd_; }
void Channel::setFd(int fd) { fd_ = fd; }

void Channel::handleEvents() {
  events_ = 0;  // 置零操作
  if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)) {
    events_ = 0;
    return;
  }
  if (revents_ & EPOLLERR) {
    if (errorHandler_) errorHandler_();
    events_ = 0;
    return;
  }
  if (revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {
    if (readHandler_) {
      readHandler_();
    }
  }
  if (revents_ & EPOLLOUT) {
    if (writeHandler_) {
      writeHandler_();
    }
  }
  if (connHandler_) {
    connHandler_();
  }
}