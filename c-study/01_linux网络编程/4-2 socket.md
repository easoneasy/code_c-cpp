
网络字节序与本机字节序的转换(大端--小端)
```
htonl   本机-->网络  一般ip使用
htons   本机-->网络  一般端口使用
ntohl
ntohs
```


点分十进制转成网络字节序  struct in_addr
```c
struct sockaddr_in addr,client_addr;

memset(&addr,0,sizeof(addr));
addr.sin_family=AF_INET;
addr.sin_addr.s_addr=inet_addr(ip);
addr.sin_port=htons(atoi(port));
atoi 是将字符串转成整型
```


使用TCP协议传输数据

![[TCP通信流程图.png]]

服务端需要创建socket对象并返回一个文件描述符
```c
int socket_fd=socket(AF_INET,SOCK_STREAM,0);
```
绑定ip和端口号 
```c
//创建结构体，里面有ip和port字段
struct sockaddr_in addr,client_addr;

memset(&addr,0,sizeof(addr));

addr.sin_family=AF_INET;
addr.sin_addr.s_addr=inet_addr(ip);
addr.sin_port=htons(atoi(port));
//绑定ip和端口号
//创建的结构体类型是sockaddr_in，这里需要强制类型转换成sockaddr
int ret = bind(socket_fd,(struct sockaddr *)&addr,sizeof(addr));
```
监听ip和端口号
半连接队列存放：成功握手一次的连接
全连接队列存放：完成三次握手的连接
```c
//第一个参数：socket端点文件描述符 
//第二个参数：套接字可以挂起的最大连接数
listen(socket_fd,10);
```

使用accept函数从服务端的socket端点的全连接队列中取出一个连接
```c
//第二个参数：获取对端的地址信息，如果不需要就设为NULL
//第三个参数：用来获取addr结构体的大小，sizeof(addr) 如果第二个参数是NULL，这个也设置成NULL
int conn_fd=accept(socket_fd,(struct sockaddr *)&client_addr,&len);
//返回一个新的套接字文件描述符用于与对端通信
```

再进行收发操作recv、send
```c
char buff[100]={0};
//第一个参数是：向这个连接的文件描述符发送数据
int cnt = recv(conn_fd,buff,sizeof(buff),0);
cnt = send(conn_fd,"hello",5,0);
```
关闭连接、关闭socket对象返回的文件描述符
```c
close();
```
还可能用到端口复用
```c
//不需要改参数
int opt =1;    
setsockopt(lisfd,SOL_SOCKET,SO_REUSEPORT,&opt,sizeof(opt));
```
TCP协议的四次挥手发起关闭端在发送最后一个ACK请求之后不会立马关闭连接，而是进入TIME_WAIT，等待2MSL时间再关闭，端口复用就是不用等待2MSL时间，再次使用该端口
-端口复用 是高效优化手段，需分场景选择：

    - 服务端重启：用 `SO_REUSEADDR`（代码可控）；
    - 客户端短连接：开 `tcp_tw_reuse`（内核优化）；

任何复用都需保证 “**旧连接已彻底终结**”，并做好网络环境校验。


客户端这边使用TCP协议收发数据：

需要创建socket对象并返回一个文件描述符
```c
int socket_fd=socket(AF_INET,SOCK_STREAM,0);
```
设置好服务器的ip和端口号
```c
char *ip="192.168.254.128";
 char *port="12345";
 
struct sockaddr_in addr;
memset(&addr,0,sizeof(addr));
addr.sin_family=AF_INET;
addr.sin_addr.s_addr=inet_addr(ip);
addr.sin_port=htons(atoi(port));
```
发起和建立连接connect
```c
int ret = connect(socket_fd,(struct sockaddr *)&addr,sizeof(addr));
```

使用recv和send收发数据
```c
//返回值都是发送/接收到的数据的个数
send(socket_fd,buff1,strlen(buff1),0);

recv(socket_fd,buff,sizeof(buff),0);
```


##### UDP网络通信

![[UDP通信流程图.png]]

服务器创建socket对象，返回文件描述符
```c
int lisfd = socket(AF_INET,SOCK_DGRAM,0);
```
绑定自己的ip和端口号，设置客户端存储ip和端口号的结构体，客户端发来数据，可将客户端的ip和端口号存储，也可以不设置，将sendto、recvfrom接收客户端的结构体设置为NULL
```c
bind(lisfd,(struct sockaddr *)&addr,sizeof(addr));
```
收发数据

关闭socket返回值的文件描述符


客户端创建socket对象
```c
int fd = socket(AF_INET, SOCK_DGRAM, 0);
```
向服务器收发数据
需要知道服务器的ip地址和端口号
```c
sendto(fd, "hello", 5, 0, (struct sockaddr *)&addr, sizeof(addr));

socklen_t len = sizeof(addr);   
recvfrom(fd, buff, 1024, 0, (struct sockaddr *)&addr, &len);
```
关闭文件描述符



recvfrom
```c
//1:socket端点文件描述符
//3：默认为0
//要发送的目的ip和端口号的结构体
//结构体的长度,一边存储该目的的信息
recvfrom(lisfd,buff1,sizeof(buff1),0,(struct sockaddr *)&client_addr,&len);
//socklen_t len = sizeof(client_addr);
```
sendto
```c
//1:socket端点文件描述符
//3：默认为0
//要发送的目的ip和端口号的结构体
//结构体的长度
sendto(lisfd,"nihao",5,0,(struct sockaddr *)&client_addr,sizeof(client_addr));
```


