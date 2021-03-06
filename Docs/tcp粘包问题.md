## 什么是tcp粘包
tcp粘包就是指发送方发送的若干包数据到达接收方时粘成了一包，从接收缓冲区来看，后一包数据的头紧接着前一包数据的尾，出现粘包的原因是多方面的，可能是来自发送方，也可能是来自接收方。
## 造成tco粘包的原因
1.  
2. 当发送内容较大时，由于服务器端的recv（buffer_size）方法中的buffer_size较小，不能一次性完全接收全部内容，因此在下一次请求到达时，接收的内容依然是上一次没有完全接收完的内容，因此造成粘包现象。
**接收方不知道该接收多大的数据才算接收完毕，造成粘包**

## 什么时候需要处理粘包现象
1. 如果发送方发送的多个分组本来就是同一个数据的不同部分，比如一个很大的文件被分成多个分组发送，这时，当然不需要处理粘包的现象；
2. 但如果多个分组本毫不相干，甚至是并列的关系，我们就一定要处理粘包问题了。比如，我当时要接收的每个分组都是一个有固定格式的商品信息，如果不处理粘包问题，每个读进来的分组我只会处理最前边的那个商品，后边的就会被丢弃。这显然不是我要的结果。

## 如何处理粘包现象
1. 发送方
对于发送方造成的粘包现象，我们可以通过关闭Nagle算法来解决，使用TCP_NODELAY选项来关闭Nagle算法。
2. 接收方
遗憾的是TCP并没有处理接收方粘包现象的机制，我们只能在应用层进行处理。
3. 应用层
应用层的处理简单易行！并且不仅可以解决接收方造成的粘包问题，还能解决发送方造成的粘包问题。
解决方法就是循环处理：应用程序在处理从缓存读来的分组时，读完一条数据时，就应该循环读下一条数据，直到所有的数据都被处理；但是如何判断每条数据的长度呢？  


## 解决办法
1. 发送定长包
2. 包尾加上\r\n标记
3. 包头加上包体长度
4. 使用更加复杂的应用层协议
