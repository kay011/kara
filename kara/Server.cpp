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
    acceptChannel_ -> setFd(listenfd_);
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
    acceptChannel_ -> setConnHandler(std::bind(&Server::hanThisConn, this));
    loop_ -> addToPoller(acceptChannel_, 0);
    started_ = true;
}

void Server::handNewConn(){
    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(struct sockaddr_in));
    socklen_t client_addr_len = sizeof(client_addr);

    int accept_fd = 0;

    while((accept_fd = accept(listenfd_, (struct sockaddr*)&client_addr, &client_addr_len)) > 0){
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

        std::shared_ptr<HttpData> req_info(new HttpData(loop, accept_fd));

        req_info -> getChannel() -> setHolder(req_info);
        loop -> queueInLoop(std::bind(&HttpData::newEvent, req_info));

    }
    acceptChannel_->setEvents(EPOLLIN | EPOLLET);

}