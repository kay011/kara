#pragma once
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <unistd.h>
#include <sys/timerfd.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <stdint.h>
#include <pthread.h>
#include <iostream>
#include <map>
#include <algorithm>
#include <list>

class Timer{
public:
    Timer();
    ~Timer();

    typedef void (*CALLBACK_FN)(void *);
    typedef struct _TimerEvent{
        int fd;
        CALLBACK_FN cbf;
        void *args;
    }TimerEvent;

    bool start(const uint interval, CALLBACK_FN cbf, void *args, const bool triggerd_on_start=false);

    void stop();
private:
    bool m_is_start;
    TimerEvent m_te;
};