/**
 * @Author Karate Yuan
 * @Email haodong_yuan@163.com
 * @Date: 2020/6/7
 */
#pragma once
#include <sys/epoll.h>
#include <unistd.h>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include "Timer.h"

// 前向声明
class EventLoop;
class TimerNode;
class Channel;

// 主状态机  ？？？
enum ProcessState{
    STATE_PARSE_URI = 1,
    STATE_PARSE_HEADERS,
    STATE_RECV_BODY,
    STATE_ANALYSIS,
    STATE_FINISH
};

enum URIState{
    PARSE_URI_AGAIN = 1,
    PARSE_URI_ERROR,
    PARSE__URI_SUCCESS
};

enum HeaderState{
    PARSE_HEADER_SUCCESS = 1,
    PARSE_HEADER_AGAIN,
    PARSE_HEADER_ERROR
};

enum AnalysisState{ ANALYSIS_SUCCESS = 1, ANALYSIS_ERROR };

enum ParseState{
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

enum ConnectionState{ H_CONNECTIONED = 0, H_DISCONNECTING, H_DISCONNECTED };

enum HttpMethod{ METHOD_POST = 1, METHOD_GET, METHOD_HEAD };
enum HttpVersion {HTTP_10 = 1, HTTP_11};

class MimeType{
public:
    static std::string getMime(const std::string &suffix);
private:
    static pthread_once_t once_control;

private:
    static void init();
    static std::unordered_map<std::string, std::string> mime;
    MimeType();
    MimeType(const MimeType& m);


};

// 为什么这里需要enable_shared_from_this
// 如果HttpData类型的对象httpData是被std::shared_ptr 管理的
// 类型HttpData继承了enable_shared_from_this，那么HttpData就有一个shared_from_this的成员函数
// 这个函数返回一个新的std::shared_ptr 的对象，也指向对象httpData。
class HttpData : public std::enable_shared_from_this<HttpData>{
public:
    // 构造函数
    HttpData(EventLoop *loop, int connfd);
    // 析构函数
    ~HttpData(){ close(fd_);}

    void reset();
    void seperateTimer();
    void linkTimer(std::shared_ptr<TimerNode> mtimer){
        timer_ = mtimer;
    }

    std::shared_ptr<Channel> getChannel(){ return channel_; }
    EventLoop *getLoop(){ return loop_; }
    void handleClose();
    void newEvent();

private:
    EventLoop *loop_;
    std::shared_ptr<Channel> channel_;
    int fd_;
    std::string inBuffer_;
    std::string outBuffer_;
    bool error_;
    ConnectionState connectionState_;

    HttpMethod method_;
    HttpVersion HTTPVersion_;
    std::string filename_;
    std::string path_;
    int nowReadPos_;
    ProcessState state_;
    ParseState hState_;
    bool keepAlive_;
    std::map<std::string, std::string> headers_;
    // 为什么用weak_ptr
    std::weak_ptr<TimerNode> timer_;

    void handleRead();
    void handleWrite();
    void handleConn();
    void handleError(int fd, int err_num, std::string short_msg);
    URIState parseURI();
    HeaderState parseHeaders();
    AnalysisState analysisRequest();

};