# 本文总结 setsockopt与getsockpot API的使用
`int getsockopt(int sock, int level, int optname, void *optval, socklen_t *optlen)`;
`int setsockopt(int sock, int level, int optname, const void *optval, socklen_t optlen)`;
参数: 
- sock: 将要被设置或者选项的套接字
- level: 选项所在的协议层，可以取三种值: 1. SOL_SOCKET: 通用套接字选项；2. IPPROTO_IP: IP 选项  3. IPPROTO_TCP: TCP选项
- optname: 想要访问的选项名
- optval: 对于getsockopt(), 指向返回选项值的缓冲。对于setsockopt(), 指向包含新选项值的缓冲。
- optlen: 对于getsockopt(), 作为入口参数时， 选项值的最大长度。作为出口参数时，选项值的实际长度。对于setsockopt(), 选项的长度。


下面介绍几个 level 为 SOL_SOCKET(通用套接字选项)的optname
- SO_BROADCAST   允许发送广播数据
- SO_KEEPALIVE   保持连接
- SO_LINGER      延迟关闭连接  struct linger
- SO_REVBUF      接收缓冲区大小
- SO_SNDBUF      发送缓冲区大小
- SO_REUSEADDR   允许重用本地地址和端口
下面介绍几个 level 为IPPROTO_TCP(TCP选项)的optname
- TCP_NODELAY    不使用Nagle算法
- TCP_MAXSEG     TCP最大数据段的长度
