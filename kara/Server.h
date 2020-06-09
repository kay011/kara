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
    void hanThisConn(){ loop_ -> updatePoller(acceptChannel_); }

public:
    EventLoop* loop_;
    int threadNum_;
    std::unique_ptr<EventLoopThreadPool> EventLoopThreadPool_;
    bool started_;
    std::shared_ptr<Channel> acceptChannel_;
    int port_;
    int listenfd_;
    static const int MAXFDS = 100000;
}