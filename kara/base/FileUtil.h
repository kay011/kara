/**
 * @Author Karate Yuan
 * @Email haodong_yuan@163.com
 * @Date: 2020/5/28
 */

#pragma once
#include <string>
#include "noncopyable.h"
// 功能:
// 打开一个文件： 往文件中添加一行
class AppendFile : noncopyable {
 public:
  explicit AppendFile(std::string filename);
  ~AppendFile();
  void append(const char* logline, const size_t len);
  void flush();

 private:
  size_t write(const char* logline, size_t len);
  FILE* fp_;
  char buffer_[64 * 1024];
};