/**
 * @Author Karate Yuan
 * @Email haodong_yuan@163.com
 * @Date: 2020/5/29
 */

#include "AsyncLogging.h"
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <functional>
#include "LogFile.h"

AsyncLogging::AsyncLogging(std::string basename, int flushInterval)
    : flushInterval_(flushInterval),
      running_(false),
      basename_(basename),
      thread_(std::bind(&AsyncLogging::threadFunc, this), "Logging"),
      mutex_(),
      cond_(mutex_),
      currentBuffer_(new Buffer),
      nextBuffer_(new Buffer),
      buffers_(),
      latch_(1) {
  assert(basename.size() > 1);
  currentBuffer_->bzero();
  nextBuffer_->bzero();
  buffers_.reserve(16);
}
// 前端
void AsyncLogging::append(const char* logline, int len) {
  // 是否需要等待thread_func 启动呢
  MutexLockGuard lock(mutex_);
  if (currentBuffer_->avail() > len) {
    // 最常见的场景: 当前缓冲区没满，复制数据到这里
    currentBuffer_->append(logline, len);
  } else {
    // buffer满了， push it， 寻找下一块空闲buffer
    buffers_.push_back(currentBuffer_);  // 如果缓冲区满，放入buffer_
    currentBuffer_.reset();
    if (nextBuffer_) {  // 如果已经有空闲的，就用它
      currentBuffer_ = std::move(nextBuffer_);
    } else {  // 如果没有  allocate 一块新的  // 极少发生的情况
      currentBuffer_.reset(new Buffer);
    }
    currentBuffer_->append(logline, len);  // 交换完继续追加
    cond_.notify();  // 并通知后端开始写入日志数据
  }
}
// 后端 往磁盘写
void AsyncLogging::threadFunc() {
  assert(running_ == true);
  latch_.countDown();  // 保证线程启动
  LogFile output(basename_);
  BufferPtr newBuffer1(new Buffer);  // 同样设置两个buffer
  BufferPtr newBuffer2(new Buffer);
  newBuffer1->bzero();
  newBuffer2->bzero();
  BufferVector buffersToWrite;  // 一个bufferVector
  buffersToWrite.reserve(16);
  while (running_) {
    assert(newBuffer1 && newBuffer1->length() == 0);
    assert(newBuffer2 && newBuffer2->length() == 0);
    assert(buffersToWrite.empty());

    {
      MutexLockGuard lock(mutex_);
      if (buffers_.empty()) {
        cond_.waitForSeconds(flushInterval_);
      }
      buffers_.push_back(currentBuffer_);  // 间隔一段时间 放入buffers_
      currentBuffer_.reset();

      currentBuffer_ = std::move(newBuffer1);
      buffersToWrite.swap(buffers_);
      // 用newBuffer2 替换 nextBuffer_
      // 这样前段始终有一个预备buffer可供调配。
      // 可以减少前端临界区分配内存的概率，缩短前端临界区长度
      if (!nextBuffer_) {
        nextBuffer_ = std::move(newBuffer2);
      }
      // 临界区加这可以不
    }
    assert(!buffersToWrite.empty());
    // 如果日志消息堆积怎么办
    // 生产速度高于消费速度，会造成数据在内存中堆积，直接丢掉多余的日志buffer
    if (buffersToWrite.size() > 25) {
      buffersToWrite.erase(buffersToWrite.begin() + 2, buffersToWrite.end());
    }
    // 通过buffersToWrite往文件写
    for (size_t i = 0; i < buffersToWrite.size(); ++i) {
      output.append(buffersToWrite[i]->data(), buffersToWrite[i]->length());
    }

    if (buffersToWrite.size() > 2) {
      buffersToWrite.resize(2);
    }
    if (!newBuffer1) {
      assert(!buffersToWrite.empty());
      newBuffer1 = buffersToWrite.back();
      buffersToWrite.pop_back();
      newBuffer1->reset();
    }
    if (!newBuffer2) {
      assert(!buffersToWrite.empty());
      newBuffer2 = buffersToWrite.back();
      buffersToWrite.pop_back();
      newBuffer2->reset();
    }

    buffersToWrite.clear();
    output.flush();
    //    }
    // output.flush();
  }
  output.flush(); 
}