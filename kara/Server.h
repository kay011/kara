/**
 * @Author Karate Yuan
 * @Email haodong_yuan@163.com
 * @Date: 2020/6/7
 */

#pragma once
#include <memory>
#include "EventLoop.h"
#include "EventLoopThreadPool.h"

class Server{
public:
    Server(EventLoop *loop, int threadNum, int port);
    ~Server(){}

    EventLoop *getLoop() const { return loop_; }
    void start();
    void handNewConn();
    void handThisConn(){ loop_ -> updatePoller(acceptChannel_); }

private:
    EventLoop* loop_;  //EventLoop对象
    int threadNum_;
    std::unique_ptr<EventLoopThreadPool> EventLoopThreadPool_;  // 线程池对象
    bool started_;
    std::shared_ptr<Channel> acceptChannel_;  // 只负责appcet请求的 acceptChannel_
    int port_;
    int listenfd_;
    static const int MAXFDS = 100000;
};