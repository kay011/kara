/**
 * @Author Karate Yuan
 * @Email haodong_yuan@163.com
 * @Date: 2020/5/28
 */

#pragma once

#include <pthread.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <functional>
#include <memory>
#include <string>

#include "CountDownLatch.h"
#include "noncopyable.h"

class Thread : noncopyable {
 public:
  typedef std::function<void()> ThreadFunc;
  // 构造函数，显式构造，传线程需要执行的函数
  explicit Thread(const ThreadFunc&, const std::string& name = std::string());
  ~Thread();
  void start();  // 启动线程
  int join();    // 合并线程
  bool started() const { return started_; }
  pid_t tid() const { return tid_; }
  const std::string& name() const { return name_; }

 private:
  void setDefaultName();
  bool started_;   // 线程是否移动
  bool joined_;    // 线程是否已经被join
  pthread_t pthreadId_;
  pid_t tid_;  // 进程id
  ThreadFunc func_;
  std::string name_;
  CountDownLatch latch_;
};