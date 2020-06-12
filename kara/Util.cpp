/**
 * @Author Karate Yuan
 * @Email haodong_yuan@163.com
 * @Date: 2020/5/31
 */

 #include "Util.h"
 
 #include <errno.h>
 #include <fcntl.h>
 #include <netinet/in.h>
 #include <netinet/tcp.h>
 #include <signal.h>
 #include <string.h>
 #include <sys/socket.h>
 #include <unistd.h>

 const int MAX_BUFF = 4096;
 ssize_t readn(int fd, void *buff, size_t n)
 {
     size_t nleft = n;
     ssize_t nread = 0;
     ssize_t readSum = 0;

     char *ptr = (char*)buff;
     while(nleft > 0){
         if((nread = read(fd, ptr, nleft)) < 0){
             if(errno == EINTR){
                 nread = 0;
             }
             else if(errno == EAGAIN){
                 return readSum;
             }
             else{
                 return -1;
             }
         }
         else if(nread == 0){
             break;
         }
         readSum += nread;
         nleft -= nread;
         ptr += nread;
     }
     return readSum;
 }

 ssize_t readn(int fd, std::string &inBuffer, bool &zero){
     ssize_t nread = 0;
     ssize_t readSum = 0;
     while(true){
         char buff[MAX_BUFF];
         if((nread = read(fd, buff, MAX_BUFF)) < 0){
             if(errno == EINTR){
                 continue;
             }
             else if(errno == EAGAIN){
                 return readSum;
             }
             else{
                 perror("read error");
                 return -1;
             }
         }
         else if(nread == 0){
             zero = true;
             break;
         }

         readSum += nread;
         inBuffer += std::string(buff, buff + nread);
     }
     return readSum;
 }

ssize_t readn(int fd, std::string &inBuffer) {
    ssize_t nread = 0;
    ssize_t readSum = 0;
    while (true) {
        char buff[MAX_BUFF];
        if ((nread = read(fd, buff, MAX_BUFF)) < 0) {
            if (errno == EINTR)
                continue;
            else if (errno == EAGAIN) {
                return readSum;
            } 
            else {
                perror("read error");
                return -1;
            }
        } 
        else if (nread == 0) {
            // printf("redsum = %d\n", readSum);
            break;
        }
        // printf("before inBuffer.size() = %d\n", inBuffer.size());
        // printf("nread = %d\n", nread);
        readSum += nread;
        // buff += nread;
        inBuffer += std::string(buff, buff + nread);
        // printf("after inBuffer.size() = %d\n", inBuffer.size());
    }
    return readSum;
}



ssize_t writen(int fd, void *buff, size_t n){
    ssize_t nleft = n;
    ssize_t nwritten = 0;
    ssize_t writeSum = 0;
    char *ptr = (char*)buff;
    while (nleft > 0)
    {
        if((nwritten = write(fd, ptr, nleft)) <= 0){
            if(nwritten < 0){
                if(errno == EINTR){
                    nwritten = 0;
                    continue;
                }
                else if(errno == EAGAIN){
                    return writeSum;
                }
                else{
                    return -1;
                }
            }
        }
        writeSum += nwritten;
        nleft -= nwritten;
        ptr += nwritten;
    }
    return writeSum;
    
}
ssize_t writen(int fd, std::string &sbuff){
    size_t nleft = sbuff.size();
    ssize_t nwritten = 0;
    ssize_t writeSum = 0;
    const char *ptr = sbuff.c_str();
    while(nleft > 0){
        if((nwritten = write(fd, ptr, nleft)) <= 0){
            if(nwritten < 0){
                if(errno == EINTR){
                    nwritten = 0;
                    continue;
                }
                else if(errno == EAGAIN){
                    return writeSum;
                }
                else{
                    return -1;
                }
            }
        }
        writeSum += nwritten;
        nleft -= nwritten;
        ptr += nwritten;

    }
    if (writeSum == static_cast<int>(sbuff.size())){
        sbuff.clear();
    }
    else{
        sbuff = sbuff.substr(writeSum);
    }
    return writeSum;
}
// 对于Server来说，为了不被 SIGPIPE 信号杀死，那就需要忽略SIGPIPE 信号
void handle_for_sigpipe(){
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = SIG_IGN;
    sa.sa_flags = 0;
    if(sigaction(SIGPIPE, &sa, NULL)){
        return;
    }
}
int setSocketNonBlocking(int fd){
    int flag = fcntl(fd, F_GETFL, 0);
    if(flag == -1){
        return -1;
    }
    flag |= O_NONBLOCK;
    if(fcntl(fd, F_SETFL, flag) == -1){
        return -1;
    }
    return 0;
}

// Nagle算法，目前的TCP/IP默认将Nagle算法关闭，通过SO_NODELAY=1
void setSocketNodelay(int fd){
    int enable = 1;
    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (void*)&enable, sizeof(enable));
}
// 一些重要的socket选项
// SO_LINGER选项用来设置延迟关闭的时间，等待套接字发送缓冲区中的数据发送完成
// 没有设置该选项时，在调用close()后，在发送完FIN后会立即进行一些清理工作并返回。
// 如果设置了SO_LINGER选项，并且等待时间为正值，则在清理之前会等待一段时间。
void setSocketNoLinger(int fd){
    struct linger linger_;
    linger_.l_onoff = 1;
    linger_.l_linger = 30;
    setsockopt(fd, SOL_SOCKET, SO_LINGER, (const char*)&linger_, sizeof(linger_));
}
// close() 与 shutdown的()区别
// close() 函数会关闭套接字ID， 如果有其他的进程共享着这个套接字，那么它仍然是打开的
// 这个连接仍然可以用来读和写，并且有时候这是非常重要的，特别是对于多进程并发服务器来说。
// 而shutdown() 会切断进程共享的套接字的所有连接，不管这个套接字的引用计数是否为零， 
// 那些视图读的进程会接收到EOF标识，那些试图写的进程会检测到SIGPIPE信号，同时可以利用shutdown的第二个参数选择断连的方式。
// 这里放一个链接: https://www.cnblogs.com/dingxiaoqiang/p/7535015.html
void shutDownWR(int fd){
    shutdown(fd, SHUT_WR);
}
int socket_bind_listen(int port){
    // 检查port值， 取正确区间范围
    if(port < 0 || port > 65535){
        return -1;
    }
    // 创建socket(IPv4+TCP), 返回监听描述符
    int listen_fd = 0;

    if((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        return -1;
    }
    // 消除bind时， Address already in use 错误
    // TIME_WAIT？？？
    // 一般来说，一个端口释放后会等待两分钟之后才能再被使用，
    // SO_REUSEADDR是让端口释放后立即就可以被再次使用
    // 链接: https://www.cnblogs.com/HKUI/p/11707170.html
    int optval = 1;
    if(setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1){
        close(listen_fd);
        return -1;
    }

    struct sockaddr_in server_addr;
    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons((unsigned short)port);
    if (bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        close(listen_fd);
        return -1;
    }
    // 开始监听，最大等待队列长为LISTENQ
    if (listen(listen_fd, 2048) == -1) {
        close(listen_fd);
        return -1;
    }

    // 无效监听描述符
    if (listen_fd == -1) {
        close(listen_fd);
        return -1;
    }
    return listen_fd;

}


