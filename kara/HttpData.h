/**
 * @Author Karate Yuan
 * @Email haodong_yuan@163.com
 * @Date: 2020/6/12
 */
// 相当于muduo中的TcpConnection
// 里面包含有Channel
#pragma once
#include <sys/epoll.h>
#include <unistd.h>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include "Timer.h"

class EventLoop;
class TimerNode;
class Channel;
// 多个状态机
// 处理对应的多种状态
enum ProcessState {
  STATE_PARSE_URI = 1,
  STATE_PARSE_HEADERS,
  STATE_RECV_BODY,
  STATE_ANALYSIS,
  STATE_FINISH
};
// URI 对应的状态
enum URIState {
  PARSE_URI_AGAIN = 1,
  PARSE_URI_ERROR,
  PARSE_URI_SUCCESS,
};
// 请求头对应的状态
enum HeaderState {
  PARSE_HEADER_SUCCESS = 1,
  PARSE_HEADER_AGAIN,
  PARSE_HEADER_ERROR
};

enum AnalysisState { ANALYSIS_SUCCESS = 1, ANALYSIS_ERROR };

// 解析首部对应的状态
enum ParseState {
  H_START = 0,
  H_KEY,
  H_COLON,
  H_SPACES_AFTER_COLON,
  H_VALUE,
  H_CR,
  H_LF,
  H_END_CR,
  H_END_LF
};
// 连接状态: 断开连接，半连接，全连接
enum ConnectionState { H_CONNECTED = 0, H_DISCONNECTING, H_DISCONNECTED };

enum HttpMethod { METHOD_POST = 1, METHOD_GET, METHOD_HEAD };

enum HttpVersion { HTTP_10 = 1, HTTP_11 };

class MimeType {
 private:
  static void init();
  // 考虑换成map
  static std::unordered_map<std::string, std::string> mime;
  MimeType();                   // 有啥用
  MimeType(const MimeType &m);  // 拷贝构造  有啥用？

 public:
  static std::string getMime(const std::string &suffix);

 private:
  static pthread_once_t once_control;
};

class HttpData : public std::enable_shared_from_this<HttpData> {
 public:
  // 外部接口
  HttpData(EventLoop *loop, int connfd);
  ~HttpData() { close(fd_); }

  void reset();  // 重置，状态重置

  void seperateTimer();  // 和定时器分离
  // 和定时器连接
  void linkTimer(std::shared_ptr<TimerNode> mtimer) {
    // shared_ptr重载了bool, 但weak_ptr没有
    timer_ = mtimer;
  }
  // 获取对应的Channel
  std::shared_ptr<Channel> getChannel() { return channel_; }

  // 
  EventLoop *getLoop() { return loop_; }
  // 处理关闭
  void handleClose(); // 关闭HTTP连接
  // 处理新事件
  void newEvent();   // 处理新事件

 private:
  EventLoop *loop_;
  std::shared_ptr<Channel> channel_;  // 使用传进来的loop构造Channel
  int fd_;                 // 只要有fd, 就有对应的Channel
  std::string inBuffer_;   // 用的string 输入缓冲区， 读取到inBuffer中
  std::string outBuffer_;  // 输出缓冲区
  bool error_;
  ConnectionState connectionState_;

  HttpMethod method_;
  HttpVersion HTTPVersion_;
  std::string fileName_;
  std::string path_;
  int nowReadPos_;
  ProcessState state_;
  ParseState hState_;
  bool keepAlive_;           // 长短连接
  std::map<std::string, std::string> headers_;
  std::weak_ptr<TimerNode> timer_;  // 存放一个定时器节点

  void handleRead();  // 接收请求，处理请求
  void handleWrite(); // 往socket写数据
  void handleConn();  // 处理连接
  void handleError(int fd, int err_num, std::string short_msg);

  URIState parseURI();
  HeaderState parseHeaders();
  AnalysisState analysisRequest();
};