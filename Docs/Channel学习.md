# Channel学习

一个Channel对应于一个fd，这个fd可以是listenfd, eventfd, 以及HttpData里面的fd 
因此我们可以看到 在Server类中有listenfd， 就要有对应的acceptChannel, 然后为accpetChanel绑定响应的回调 Server::handNewConn和Server::handThisConn
在EventLoop类中有eventfd，就要有对应的pwakeUpChannel，然后绑定相应的EventLoop::handleRead和EventLoop::handleConn

