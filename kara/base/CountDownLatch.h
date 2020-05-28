/**
 * @Author Karate Yuan
 * @Email haodong_yuan@163.com
 * @Date: 2020/5/28
 */

#pragma once

#include "Condition.h"


// CountDownLatch的主要作用是确保Thread中传进去的func真的启动了以后
// 外层的start才返回
class CountDownLatch : noncopyable{
public:
    explicit CountDownLatch(int count);
    void wait();
    void countDown();


private:

    mutable MutexLock mutex_;
    Condition condition_;
    int count_;

};

