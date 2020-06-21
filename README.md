# kara
This is my kara server

## 项目目的
本项目是为了学习Linux环境下的服务器开发和C++多线程编程，参考了muduo的实现原理，参考了Github开源项目: WebServer。

## 项目整体结构
整体架构遵循 muduo提出的**one eventloop per thread**。遵循Reactor设计模式，其中EventLoop类相当于Reactor，IO复用采用Epoll，同时抽象出Channel类，每个Channel类对应于一个文件描述符以及对应的事件，相当于通道。EventLoop通过调用成员epoll对事件进行注册，修改，删除。然后在loop中调用每个channel的handleEvent处理不同事件。每个持有Channel的类可以有其相应的事件处理函数。



## 待解决
- 配置文件解析
- webbench压测
- POST请求处理
- 表单提交
- 文件上传
- 文件下载

