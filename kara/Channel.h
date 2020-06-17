/**
 * @Author Karate Yuan
 * @Email haodong_yuan@163.com
 * @Date: 2020/6/12
 */
// 有点乱，重构下？
#pragma once
#include <sys/epoll.h>
#include <sys/epoll.h>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include "Timer.h"

class EventLoop;
class HttpData;

/**
 * Channel 的生命周期由其owner class 负责管理，它一般是其他class的直接或间接成员。
 * Channel相当于一个通道，封装了fd
 * 每个Channel都属于一个eventLoop
 * 通过向Channel注册回调函数来处理不同的事件
 * Channel的成员函数都
 */ 
class Channel {
 private:
  typedef std::function<void()> CallBack;
  EventLoop *loop_;
  int fd_;
  __uint32_t events_;
  __uint32_t revents_;
  __uint32_t lastEvents_;

  // 方便找到上层持有该Channel的对象
  std::weak_ptr<HttpData> holder_;

 private:
  CallBack readHandler_;
  CallBack writeHandler_;
  CallBack errorHandler_;
  CallBack connHandler_;

 public:
  Channel(EventLoop *loop);
  Channel(EventLoop *loop, int fd);
  ~Channel() {}
  int getFd();
  void setFd(int fd);

  void setHolder(std::shared_ptr<HttpData> holder) { holder_ = holder; }
  std::shared_ptr<HttpData> getHolder() {
    std::shared_ptr<HttpData> ret(holder_.lock());
    return ret;
  }

  void setReadHandler(CallBack &&readHandler) { readHandler_ = readHandler; }
  void setWriteHandler(CallBack &&writeHandler) {
    writeHandler_ = writeHandler;
  }
  void setErrorHandler(CallBack &&errorHandler) {
    errorHandler_ = errorHandler;
  }
  void setConnHandler(CallBack &&connHandler) { connHandler_ = connHandler; }
  // 核心 功能是根据revents的值分别调用不同的用户回调。
  void handleEvents(); // 被EventLoop::loop()调用

  void setRevents(__uint32_t ev) { revents_ = ev; }

  void setEvents(__uint32_t ev) { events_ = ev; }
  __uint32_t &getEvents() { return events_; }
  // lastEvents是否和events相等
  // 更新lastEvents
  bool EqualAndUpdateLastEvents() {
    bool ret = (lastEvents_ == events_);
    lastEvents_ = events_;
    return ret;
  }

  __uint32_t getLastEvents() { return lastEvents_; }
};
// 是要被别的class调用的
typedef std::shared_ptr<Channel> SP_Channel;