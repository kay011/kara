/**
 * @Author Karate Yuan
 * @Email haodong_yuan@163.com
 * @Date: 2020/5/29
 */
#pragma once

#include <memory>
#include <string>
#include "FileUtil.h"
#include "MutexLock.h"
#include "noncopyable.h"

// 提供自动归档功能
class LogFile : noncopyable {
 public:
  // 每被append flushEveryN次，flush一下，会往文件写，只不过，文件也是带缓冲区的
  LogFile(const std::string& basename, int flushEveryN = 1024);
  ~LogFile() = default;

  void append(const char* logline, int len);
  void flush();
  bool rollFile();  // 日志回滚 TODO

 private:
  void append_unlocked(const char* logline, int len);
  const std::string basename_;
  const int flushEveryN_;

  int count_;
  std::unique_ptr<MutexLock> mutex_;
  std::unique_ptr<AppendFile> file_;
};
