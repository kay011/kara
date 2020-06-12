/**
 * @Author Karate Yuan
 * @Email haodong_yuan@163.com
 * @Date: 2020/6/7
 */

#pragma once
#include <memory>
#include <vector>
#include "EventLoopThread.h"
#include "base/Logging.h"
#include "base/noncopyable.h"

class EventLoopThreadPool : noncopyable{
public:
    EventLoopThreadPool(EventLoop* baseLoop, int numThreads);
    ~EventLoopThreadPool(){ LOG << "~EventLoopThreadPool()"; }

    void start();
    EventLoop* getNextLoop();
private:
    EventLoop* baseLoop_;  // 有一个baseLoop_
    bool started_;
    int numThreads_;
    int next_;
    std::vector<std::shared_ptr<EventLoopThread>> threads_; // 这里放的不是普通的线程，而是只跑eventLoop的线程
    std::vector<EventLoop*> loops_;  // 一堆EventLoop，相当于Task 
};