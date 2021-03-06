/**
 * @Author Karate Yuan
 * @Email haodong_yuan@163.com
 * @Date: 2020/5/28
 */

#include <assert.h>
#include <errno.h>
#include <linux/unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <unistd.h>
#include <memory>

#include <iostream>

#include "CurrentThread.h"
#include "Thread.h"

using namespace std;

namespace CurrentThread {
__thread int t_cachedTid = 0;
__thread char t_tidString[32];
__thread int t_tidStringLength = 6;
__thread const char* t_threadName = "default";
}

pid_t gettid() {
  // 通过系统调用得到tid，因为glibc没有实现gettid()函数
  return static_cast<pid_t>(::syscall(SYS_gettid));
}

void CurrentThread::cacheTid() {
  if (t_cachedTid == 0) {
    t_cachedTid = gettid();
    t_tidStringLength =
        snprintf(t_tidString, sizeof(t_tidString), "%5d ", t_cachedTid);
  }
}

// 线程参数，放入 pthread_create中 第四个参数
// 运行函数的参数
struct ThreadData {
  typedef Thread::ThreadFunc ThreadFunc;
  ThreadFunc func_;

  string name_;
  pid_t* tid_;
  CountDownLatch* latch_;

  ThreadData(const ThreadFunc& func, const string& name, pid_t* tid,
             CountDownLatch* latch)
      : func_(func), name_(name), tid_(tid), latch_(latch) {}

  void runInThread() {
    *tid_ = CurrentThread::tid();
    tid_ = NULL;   // 指针指向空
    latch_->countDown();  // 这里进行了countDown
    latch_ = NULL;        // 指针指向空

    CurrentThread::t_threadName = name_.empty() ? "Thread" : name_.c_str();
    prctl(PR_SET_NAME, CurrentThread::t_threadName);  // 设置进程名

    func_();  // 线程执行函数

    CurrentThread::t_threadName = "finished";
  }
};

void* startThread(void* obj) {
  ThreadData* data = static_cast<ThreadData*>(obj);
  data->runInThread();

  delete data;
  return NULL;
}

// 成员变量
Thread::Thread(const ThreadFunc& func, const string& n)
    : started_(false),
      joined_(false),
      pthreadId_(0),
      tid_(0),
      func_(func),
      name_(n),
      latch_(1) {
  setDefaultName();
}

Thread::~Thread() {
  if (started_ && !joined_) {
    pthread_detach(pthreadId_);
  }
}

void Thread::setDefaultName() {
  if (name_.empty()) {
    char buf[32];
    snprintf(buf, sizeof(buf), "Thread");
    name_ = buf;
  }
}

void Thread::start() {
  assert(!started_);
  started_ = true;
  ThreadData* data = new ThreadData(func_, name_, &tid_, &latch_);
  // pthread_create是一个线程阻塞的函数，调用它的函数将一直
  // 等待到被等待的线程结束为止，当函数返回时，被等待线程的资源被收回。
  // 如果执行成功则返回0， 如果失败则返回一个错误号。
  if (pthread_create(&pthreadId_, NULL, &startThread, data)) {
    // 启动失败，释放资源
    started_ = false;
    delete data;
  } else {
    // 启动成功
    // latch_.wait() 保证 runInThread 真的被启动了才 退出start()
    latch_.wait();
    assert(tid_ > 0);
  }
}

int Thread::join() {
  assert(started_);
  assert(!joined_);
  joined_ = true;
  return pthread_join(pthreadId_, NULL);
}
