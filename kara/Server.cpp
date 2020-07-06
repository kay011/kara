/**
 * @Author Karate Yuan
 * @Email haodong_yuan@163.com
 * @Date: 2020/6/12
 */

#include "Server.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <functional>
#include "Util.h"
#include "base/Logging.h"

Server::Server(EventLoop *loop, int threadNum, int port)
    : loop_(loop),
      threadNum_(threadNum),
      eventLoopThreadPool_(new EventLoopThreadPool(loop_, threadNum)),
      started_(false),
      acceptChannel_(new Channel(loop_)),
      port_(port),
      listenFd_(socket_bind_listen(port_)) {
  // 构造函数
  // 将listenFd_ 绑定到accpetChannel_
  acceptChannel_->setFd(listenFd_);
  handle_for_sigpipe();
  // 将listenFd 设为非阻塞
  // 为什么 因为listenFd_ 也要交给Epoll管理
  if (setSocketNonBlocking(listenFd_) < 0) {
    perror("set socket non block failed");
    abort();
  }
}

// mainReactor 
// loop_ 只appcet新的连接请求
void Server::start() {
  eventLoopThreadPool_->start();  // 启动线程池
  // acceptChannel_->setEvents(EPOLLIN | EPOLLET | EPOLLONESHOT);
  // 对于accpet线程注册 相应的回调
  acceptChannel_->setEvents(EPOLLIN | EPOLLET);
  acceptChannel_->setReadHandler(bind(&Server::handNewConn, this));
  acceptChannel_->setConnHandler(bind(&Server::handThisConn, this));  // listenfd_
  loop_->addToPoller(acceptChannel_, 0);  // loop_ 只负责 accpetChannel_ timeout=0表示不加定时器
  started_ = true;
}

// 回调函数
// Acceptor
// NewConnectionCallback
void Server::handNewConn() {
  struct sockaddr_in client_addr;
  memset(&client_addr, 0, sizeof(struct sockaddr_in));
  socklen_t client_addr_len = sizeof(client_addr);
  int connect_fd = 0;
  // ET模式
  // ET模式下，由于listenFd_ 是非阻塞的，所有要while循环
  while ((connect_fd = accept(listenFd_, (struct sockaddr *)&client_addr,
                             &client_addr_len)) > 0) {
    EventLoop *loop = eventLoopThreadPool_->getNextLoop();
    LOG << "New connection from " << inet_ntoa(client_addr.sin_addr) << ":"
        << ntohs(client_addr.sin_port);
    // cout << "new connection" << endl;
    // cout << inet_ntoa(client_addr.sin_addr) << endl;
    // cout << ntohs(client_addr.sin_port) << endl;
    /*
    // TCP的保活机制默认是关闭的
    int optval = 0;
    socklen_t len_optval = 4;
    getsockopt(connect_fd, SOL_SOCKET,  SO_KEEPALIVE, &optval, &len_optval);
    cout << "optval ==" << optval << endl;
    */
    // 限制服务器的最大并发连接数
    if (connect_fd >= MAXFDS) {
      close(connect_fd);
      continue;
    }
    // 设为非阻塞模式
    if (setSocketNonBlocking(connect_fd) < 0) {
      LOG << "Set non block failed!";
      // perror("Set non block failed!");
      return;
    }

    setSocketNodelay(connect_fd);
    // setSocketNoLinger(accept_fd);
    // 对于新来的连接，从线程池取出一个线程
    // 多个HttpData可以对应同一个EventLoop
    shared_ptr<HttpData> req_info(new HttpData(loop, connect_fd));
    req_info->getChannel()->setHolder(req_info);  // 关联
    // 为什么用queueInLoop
    // 为什么是先queue进来呢？ 因为这里是appcet事件，所以放在队列里
    // 如果有IO, 就用对应的IO 线程处理
    // 为啥不用runInLoop
    // loop->queueInLoop(std::bind(&HttpData::newEvent, req_info));
    loop->runInLoop(std::bind(&HttpData::newEvent, req_info));
  }
  acceptChannel_->setEvents(EPOLLIN | EPOLLET);  // 关心的事件
}