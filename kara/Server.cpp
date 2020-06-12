/**
 * @Author Karate Yuan
 * @Email haodong_yuan@163.com
 * @Date: 2020/6/7
 */

#include "Server.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <functional>
#include "Util.h"
#include "base/Logging.h"

Server::Server(EventLoop* loop, int threadNum, int port)
    : loop_(loop),
      threadNum_(threadNum),
      EventLoopThreadPool_(new EventLoopThreadPool(loop_, threadNum_)),
      started_(false),
      acceptChannel_(new Channel(loop_)),
      port_(port),
      listenfd_(socket_bind_listen(port_))
{
    acceptChannel_ -> setFd(listenfd_);  // 关联fd_
    handle_for_sigpipe();
    if(setSocketNonBlocking(listenfd_) < 0){
        perror("set socket non block failed");
        abort();
    }

}

void Server::start(){
    EventLoopThreadPool_ -> start();
    acceptChannel_ -> setEvents(EPOLLIN | EPOLLET);
    acceptChannel_ -> setReadHandler(std::bind(&Server::handNewConn, this));
    acceptChannel_ -> setConnHandler(std::bind(&Server::handThisConn, this));
    // 把acceptChannel_关联的fd_ 交给 poll管理
    loop_ -> addToPoller(acceptChannel_, 0);  // accpetChannel_ 关联了listenfd_
    started_ = true;
}

void Server::handNewConn(){
    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(struct sockaddr_in));
    socklen_t client_addr_len = sizeof(client_addr);

    int accept_fd = 0;

    while((accept_fd = accept(listenfd_, (struct sockaddr*)&client_addr, &client_addr_len)) > 0){
        // 每 得到一个accept_fd, 从线程池取出一个线程? 不，取出一个EventLoop
        EventLoop *loop = EventLoopThreadPool_ -> getNextLoop();
        LOG << "New Connection from " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port);
        
        // 限制服务器的最大并发数
        if(accept_fd >= MAXFDS){
            close(accept_fd);
            continue;
        }
        // 设为非阻塞模式
        if(setSocketNonBlocking(accept_fd) < 0){
            LOG << "Set non block failed!";
            return;
        }
        setSocketNodelay(accept_fd);

        // 创建一个HttpData
        std::shared_ptr<HttpData> req_info(new HttpData(loop, accept_fd));
        // 为新的连接 创建一个新的channel, 这个channel对象的持有是该HttpData
        req_info -> getChannel() -> setHolder(req_info);
        // 把这个连接放入队列中？
        
        loop -> queueInLoop(std::bind(&HttpData::newEvent, req_info));

    }
    acceptChannel_->setEvents(EPOLLIN | EPOLLET);
}