# Socket
## 字节存储顺序
Internet上数据以高位字节优先顺序在网络上传输，而在机器上是以低位字节优先方式存储数据的机器，在Internet上传输数据需要进行转换。
htons() -- "Host to Network Short"; htonl() -- "Host to Network Long"
ntohs()--"Network to Host Short" ; ntohl()--"Network to Host Long"
