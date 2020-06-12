/**
 * @Author Karate Yuan
 * @Email haodong_yuan@163.com
 * @Date: 2020/6/7
 */

#pragma once
#include <functional>
#include <memory>
#include <vector>
#include "Channel.h"
#include "Epoll.h"
#include "Util.h"
#include "base/CurrentThread.h"
#include "base/Logging.h"
#include "base/Thread.h"

class EventLoop{
public:
    typedef std::function<void()> Functor;
    EventLoop();
    ~EventLoop();

    void loop();  // 核心
    void quit();
    void runInLoop(Functor&& cb);
    void queueInLoop(Functor&& cb);
    bool isInLoopThread() const { return threadId_ == CurrentThread::tid(); } // 判断
    void assertInLoopThread() { assert(isInLoopThread()); };
    void shutdown(std::shared_ptr<Channel> channel){
        shutDownWR(channel -> getFd());
    }
    void removeFromPoller(std::shared_ptr<Channel> channel){
        poller_ -> epoll_del(channel);
    }
    void updatePoller(std::shared_ptr<Channel> channel, int timeout = 0){
        poller_ -> epoll_mod(channel, timeout);
    }
    void addToPoller(std::shared_ptr<Channel> channel, int timeout = 0){
        poller_ -> epoll_add(channel, timeout);
    }

private:
    bool looping_;  // automic
    std::shared_ptr<Epoll> poller_;  // 持有一个IO复用器
    int wakeUpFd_;   // 异步唤醒线程的fd??
    bool quit_;
    bool eventHanding_;
    mutable MutexLock mutex_;
    std::vector<Functor> pendingFunctors_;
    bool callingPendingFunctors_;
    const pid_t threadId_;  // 每个线程都持有一个 eventloop对象
    std::shared_ptr<Channel> pwakeupChannel_;   // 特殊的channel？ 哪里特殊 fd特殊

    void wakeup();
    void handleRead();
    void doPendingFunctors();
    void handleConn();
};