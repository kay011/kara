/**
 * @Author Karate Yuan
 * @Email haodong_yuan@163.com
 * @Date: 2020/6/7
 */

 #include "EventLoopThread.h"
 #include <functional>

 EventLoopThread::EventLoopThread() 
    : loop_(NULL),
      exiting_(false),
      thread_(std::bind(&EventLoopThread::threadFunc, this), "ThreadLoopThread"),
      mutex_(),
      cond_(mutex_)
{}

EventLoopThread::~EventLoopThread(){
    exiting_ = true;
    if(loop_ != NULL){
        loop_ -> quit();
        thread_.join();
    }
}

EventLoop* EventLoopThread::startLoop(){
    assert(!thread_.started());
    thread_.start();
    // 临界区
    {
        MutexLockGuard lock(mutex_);
        while(loop_ == NULL){
            cond_.wait();
        }
    }
    // loop要交给线程池的
    return loop_;
}

void EventLoopThread::threadFunc(){
    EventLoop loop;
    {
        MutexLockGuard lock(mutex_);
        loop_ = &loop;
        cond_.notify();
    }
    loop.loop();
    loop_ = NULL;

}

