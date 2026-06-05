##### select
![[select原理图.png]]
1、创建一个位图集合，默认长度为1024，
想监听哪个文件描述符就把位图中对应的那一位设置为1
```c
fd_set set;
FD_ZERO(&set);
FD_SET(lisfd,&set);
```
2、调用select函数时，系统将位图从用户态拷贝到内核态
```c
int nready = select(10,&tmp,NULL,NULL,NULL);
//返回值是状态改变的文件描述符的数目，但是不告诉具体是哪个
//需要轮询查找
----------------------------------------------------
//第一个参数：文件的最大描述符值+1
//最后一个参数：select调用的最长等待时间
```
3、内核会轮询扫描位图中的文件描述符
位图中无fd就会让当前进程进入睡眠状态
有一个或多个fd产生了事件就会修改对应的位，标记
```c
if(FD_ISSET(lisfd,&tmp))
//执行对应的操作
```
4、标记完成之后将修改过的位图拷贝到用户态（替换一开始的位图）

使用select实现一个服务器与多个客户端之间进行通信，
将客户端发送过来的数据转发给其他客户端
```c
#include <my_header.h>
typedef struct conn
{
    int fd; //文件描述符
    int alive; //连接是不是存活的，活就是1，死就是0
}conn_t;

int main(int argc, char *argv[])
{
    char *ip = "192.168.100.128";
    char *port = "12345";

    //创建用于监听的文件描述符
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    ERROR_CHECK(listen_fd, -1, "socket");

    //端口复用
    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    //绑定服务器的ip与端口号
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));

    //在对结构体遍历addr进行填充
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(port));
    addr.sin_addr.s_addr = inet_addr(ip);

    int ret = bind(listen_fd, (struct sockaddr *)&addr, sizeof(addr));
    ERROR_CHECK(ret, -1, "bind");

    //监听客户端
    ret = listen(listen_fd, 10);
    ERROR_CHECK(ret, -1, "listen");

    //存放群聊中客户端的个数的数组
    conn_t con_arr[100] = {0};
    int size = 0;

    fd_set set;
    FD_ZERO(&set);
    FD_SET(listen_fd, &set);

    while(1)
    {
        fd_set tmp;//临时位图
        tmp = set;

        int nready = select(100, &tmp, NULL, NULL, NULL);
        ERROR_CHECK(ret, -1, "select");
        printf("nready: %d\n", nready);

        if(FD_ISSET(listen_fd, &tmp))
        {
            int conn_fd = accept(listen_fd, NULL, NULL);
            ERROR_CHECK(conn_fd, -1, "accept");

            con_arr[size].fd = conn_fd;
            con_arr[size].alive = 1;
            ++size;

            //需要将conn_fd放在监听集合中进行监听
            FD_SET(conn_fd, &set);
        }

        //需要遍历size
        for(int idx = 0; idx < size; ++idx)
        {
            //从数组中取出其中的值
            int fd = con_arr[idx].fd;

            //表明有数据从老的连接上发过来了
            if(FD_ISSET(fd, &tmp))
            {
                char buf[50] = {0};
                //recv的返回结果是实际接收的数据的个数
                int cnt = recv(fd, buf, sizeof(buf), 0);
                if(0 == cnt)
                {
                    printf("客户端fd = %d断开了\n", fd);
                    con_arr[idx].fd = 0;
                    con_arr[idx].alive = 0;

                    //客户端断开后就不需要进行监听了，所以需要从set中
                    //清除
                    FD_CLR(fd, &set);
                    close(fd);

                    continue;
                }

                //转发给其他的客户端
                for(int pos = 0; pos < size; ++pos)
                {
                    //转发给的其他客户端如果是断开的，就不发
                    //pos == idx说明是自己
                    if(0 == con_arr[pos].alive || pos == idx)
                    {
                        continue;
                    }
                    send(con_arr[pos].fd, buf, sizeof(buf), 0);
                }
            }
        }
    }

    close(listen_fd);

    return 0;
}
```


##### epoll与select的区别：
epoll可以监听的文件描述符数量没有限制，
将监听集合与就绪集合分开，
监听集合存放在内核态的红黑树上，
就绪集合从红黑树拷贝到内核态的双向链表中
只需将就绪的文件描述符从内核态拷贝到用户态
内核底层采取回调机制，对监听的文件描述符状态进行操作

select
监听文件描述符的数量有限1024
监听与就绪集合需要人为分开，位图会从用户态拷贝到内核态，
还要从内核态拷贝到用户态，需要监听整个位图

创建监听文件描述符
epoll_create(int size)  只要＞0即可
```c
int epfd=epoll_create(1);
```
epoll_create1

epoll_ctl
```c
struct epoll_event evt;
evt.events=EPOLLIN;
evt.data.fd=lisfd;
int ret=epoll_ctl(epfd,EPOLL_CTL_ADD,lisfd,&evt);
//EPOLL_CTL_ADD / EPOLL_CTL_DEL / EPOLL_CTL_MOD
```

```c
struct epoll_event{
	uint32_t events
	epoll_data_t data;
}
```

联合体 union
共享内存，sizeof(union) = 成员最大类型的大小
所有其他所有成员共享这一块空间，同一时刻只能用一个成员
```c
typedef union epoll_data{
	void *ptr          8字节
	int fd;            4
	uint32_t u32;      4
	uint64_t u64;      8
}epoll_data_t;
```

epoll_wait  （阻塞函数）
```c
//用于接收就绪文件描述符集合的结构体数组
struct epoll_event lst[10];
int nready = epoll_wait(epfd,lst,10,-1);
//第三个参数：最大就绪集合长度
//第四个参数：超时时间（毫秒）
//-1是一直等待直到有就绪文件描述符
```

服务器与多个客户端通信，服务器接收客户端发送来的数据并转发给其他客户端
```c
typedef struct conn_s
{
    int net_fd;
    int alive; // 1存活， 0断开链接
}conn_t;

int main(int argc,char*argv[])
{
    char *ip = "192.168.100.128";
    char *port = "12345";

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in sockaddr;
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(atoi(port));
    sockaddr.sin_addr.s_addr = inet_addr(ip);

    bind(socket_fd, (struct sockaddr *)&sockaddr, sizeof(sockaddr));
    listen(socket_fd,  20);

    conn_t conns[1000];
    int size = 0;
    memset(conns, 0, sizeof(conns));

    int epoll_fd = epoll_create(1);

    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = socket_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &event);

    while(1)
    {
        // 创建一个就绪的数组
        struct epoll_event list[5];
        int num = epoll_wait(epoll_fd, list, 5, -1);

        for(int i=0; i<num; i++)
        {
            struct epoll_event event = list[i];
            if(event.data.fd == socket_fd)
            {
                // 走到这说明有新的链接到来
                int net_fd = accept(socket_fd, NULL, NULL);

                conns[size].alive = 1;
                conns[size].net_fd = net_fd;
                size++;

                struct epoll_event event_net_fd;
                event_net_fd.events = EPOLLIN;
                event_net_fd.data.fd = net_fd;
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, net_fd,&event_net_fd);
            }
            else
            {
                char buf[60] = {0};
                int ret = recv(event.data.fd, buf, sizeof(buf), 0);
                if(ret == 0)
                {
                    // i这个客户端断开链接
                    for(int k=0; k<size; k++)
                    {
                        if(conns[k].net_fd == event.data.fd && conns[k].alive==1)
                        {
                            conns[k].alive = 0;
                            break;
                        }
                    }
                    
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event.data.fd, NULL);
                    close(event.data.fd);
                    continue;
                }
                
                for(int j=0; j<size; j++)
                {
                    if(conns[j].alive==1 && conns[j].net_fd != event.data.fd)
                    {
                        send(conns[j].net_fd, buf, sizeof(buf), 0);
                    }
                }
            }
        }
    }
    
    close(socket_fd);
    
    return 0;
}
```


##### 触发模式：
###### 1、水平触发
	默认，只要被监视的文件描述符上有待处理事件，epoll_wait就会通知应用程序，也就是文件描述符就绪了。

```c
#include <my_header.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    //水平触发式默认的模式,监听STDIN_FILENO文件描述符，只要通过终端
    //输入数据，就可以触发这个文件描述符可读
    int epfd = epoll_create(1);
    ERROR_CHECK(epfd, -1, "epoll_create");

    struct epoll_event evt;
    evt.events = EPOLLIN;//监听读事件
    evt.data.fd = STDIN_FILENO;//监听的文件描述符
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &evt);
    ERROR_CHECK(ret, -1, "epoll_ctl add");

    while(1)
    {
        //对于水平触发而言，只要文件描述符可读（在这里就是缓冲区中有数据
        //那么，就一直可读，一直将缓冲区中的数据读空）
        struct epoll_event lst[2];
        int nready = epoll_wait(epfd, lst, 2, -1);
        ERROR_CHECK(nready, -1, "epoll_wait");
        printf("nready: %d\n", nready);
        printf("----------------------------\n\n" );

        for(int idx = 0; idx < nready; ++idx)
        {
            int fd = lst[idx].data.fd;

            if(fd == STDIN_FILENO)
            {
                char buf[2] = {0};
                read(fd, buf, sizeof(buf));
                printf("buf:%s\n", buf);
            }
        }

    }
    return 0;
}
```

###### 2、边缘触发
	文件描述符的状态发生变化，想读多少数据可以自己定，不会多次触发所以效率比水平触发要高。但是数据可能一直堆积在缓冲区
	想要将数据全都读走，可以使用while循环，持续进行读操作。
	也可以结合recv的非阻塞模式使用
	recv的非阻塞模式：将最后一个标志位置设置为MSG_DONTWAIT
	
```c
#include <my_header.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    //水平触发式默认的模式,监听STDIN_FILENO文件描述符，只要通过终端
    //输入数据，就可以触发这个文件描述符可读
    int epfd = epoll_create(1);
    ERROR_CHECK(epfd, -1, "epoll_create");

    struct epoll_event evt;
    evt.events = EPOLLIN | EPOLLET;//监听读事件,并且是边缘触发
    evt.data.fd = STDIN_FILENO;//监听的文件描述符
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &evt);
    ERROR_CHECK(ret, -1, "epoll_ctl add");

    while(1)
    {
        //对于水平触发而言，只要文件描述符可读（在这里就是缓冲区中有数据
        //那么，就一直可读，一直将缓冲区中的数据读空）
        struct epoll_event lst[2];
        int nready = epoll_wait(epfd, lst, 2, -1);
        ERROR_CHECK(nready, -1, "epoll_wait");
        printf("nready: %d\n", nready);
        printf("----------------------------\n\n" );

        for(int idx = 0; idx < nready; ++idx)
        {
            int fd = lst[idx].data.fd;

            if(fd == STDIN_FILENO)
            {
                while(1)
                {
                    char buf[2] = {0};
                    read(fd, buf, sizeof(buf));
                    printf("buf:%s\n", buf);
                }
                printf("********\n");
            }
        }

    }
    return 0;
}
```

