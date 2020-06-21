/**
 * @Author Karate Yuan
 * @Email haodong_yuan@163.com
 * @Date: 2020/5/28
 */

#pragma once

#include <stdint.h>

namespace CurrentThread {
// internal
// extern是一种“外部声明”的关键字，字面意思是在此处声明某种变量或函数，在外部定义。
// 在哪里定义了呢， 答案是在 Thread.cpp 里
extern __thread int t_cachedTid;
extern __thread char t_tidString[32];
extern __thread int t_tidStringLength;
extern __thread const char* t_threadName;
void cacheTid();
inline int tid() {
  if (__builtin_expect(t_cachedTid == 0, 0)) {
    cacheTid();
  }
  return t_cachedTid;
}

inline const char* tidString() { return t_tidString; }

inline int tidStringLength() { return t_tidStringLength; }

inline const char* name() { return t_threadName; }
}
