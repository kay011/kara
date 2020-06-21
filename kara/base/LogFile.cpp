/**
 * @Author Karate Yuan
 * @Email haodong_yuan@163.com
 * @Date: 2020/5/29
 */

#include <assert.h>
#include <stdio.h>
#include <time.h>

#include "FileUtil.h"
#include "LogFile.h"

using namespace std;

LogFile::LogFile(const string& basename, int flushEveryN)
    : basename_(basename),
      flushEveryN_(flushEveryN),
      count_(0),
      mutex_(new MutexLock) {
  // unique_ptr api
  file_.reset(new AppendFile(basename));
}

void LogFile::append(const char* logline, int len) {
  MutexLockGuard lock(*mutex_);
  append_unlocked(logline, len);
}

void LogFile::flush() {
  MutexLockGuard lock(*mutex_);
  // 指针 api
  file_->flush();
}

void LogFile::append_unlocked(const char* logline, int len) {
  file_->append(logline, len);  // 非线程安全
  ++count_;
  if (count_ >= flushEveryN_) {
    count_ = 0;
    file_->flush();
  }
}