
### epoll 
核心数据结构：红黑树+双向链表
红黑树：保存所有要监控的fd，使用epoll_ctl添加fd时，被挂载到内核的红黑树上，重复监听时就不需要每次都拷贝整个fd集合，只需要增量修改，查找、删除的时间复杂度为O(logN)
双向链表：某个fd有数据可读或可写时，内核的硬件中断处理程序会触发回调函数，把这个fd放到就绪链表中

epoll_wait 检查内核的就绪链表是否为空，不为空直接把就绪的fd拷贝回用户态，时间复杂度是O(就绪连接数)，与总连接数无关

```
网络初始化与监听

初始化epoll实例

将监听套接字加入epoll

事件循环
	死等事件发生
	遍历就绪事件
	分流处理不同事件
		A新连接到达
		已连接的客户端有动静
			收到的数据个数<0 发生错误
			收到的数据个数=0 客户端主动关闭连接
				-被动断开方需要先将文件描述符从红黑树中取下
				-再关闭文件描述符
			成功收到数据 
``` 


#### 水平触发和边缘触发

水平触发：关注的是状态，只要条件满足（比如缓冲区有状态）内核就会不断通知
- 套接字的I/O模式：阻塞和非阻塞
- 高并发上限低，大量就绪的fd会导致频繁的系统调用
```
在 LT 模式下，如果某个 FD 的数据你只读了一半，或者你还没来得及读，内核在每次 epoll_wait 返回时，都要把这个 FD 留在就绪链表里，或者反复地在红黑树和就绪链表之间挪动、确认状态。
```

```
1. 水平触发（LT）的流程：
调用 epoll_wait，发现有数据。 （第 1 次内核切换）
调用 recv 读了 1KB。
循环回去，调用 epoll_wait，因为没读完，它又立刻返回。 （第 2 次内核切换）
调用 recv 读了 1KB。
循环回去，调用 epoll_wait，又立刻返回。 （第 3 次内核切换）
...
结果：为了读完这 4KB 数据，应用层一共调用了 4 次 epoll_wait 和 4 次 recv。
```

边缘触发：关注的是变化，只有当状态发生改变的一瞬间（有新数据到达时）内核才会通知一次
- 套接字的I/O模式：非阻塞
```
如果套接字是阻塞的：当读到最后一次，缓冲区空了，recv 就会死死卡在那里，整个线程直接瘫痪，再也无法处理别的客户端连接了。

如果套接字是非阻塞的：当缓冲区空了，recv 不会卡住，而是会立刻返回一个错误码 EAGAIN（意思是：现在没数据了，下次再来）。这样代码就能优雅地 break 退出循环，把 CPU 让给下一个事件。
```
- 必须使用while(recv)循环读取到返回EAGAIN为止
- 高并发上限高，减少了epoll_wait返回的次数
```
边缘触发（ET）的极简艺术：
在 ET 模式下，只要数据来了，内核把这个 FD 往就绪链表里一丢，通知你一次。
紧接着，内核不管你读没读完，立刻把这个 FD 从就绪链表里剔除（开除出队）。
此时，就绪链表变得极简、极短！内核不需要再去关心和维护这些已经通知过的 FD 状态。除非客户端又发来新数据，否则这个 FD 绝对不会再占用内核任何的清点时间。
```

```
2. 边缘触发（ET）的流程：
调用 epoll_wait，发现有数据，返回。 （第 1 次内核切换）
进入 while 循环：
第 1 次 recv 读 1KB
第 2 次 recv 读 1KB
第 3 次 recv 读 1KB
第 4 次 recv 读 1KB
第 5 次 recv 返回 EAGAIN（空了，退出循环）
循环回去，调用 epoll_wait 挂起，等待下一个新事件。
结果：为了读完 4KB，一共调用了 1 次 epoll_wait 和 5 次 recv。
```

```
在 Linux 中，epoll_wait 的上下文切换开销，远远大于纯数据的 recv
ET 模式用纯在用户态循环的 recv（只需要检查内核缓冲区，速度极快），成功抵消并减少了昂贵的 epoll_wait 系统调用次数。在百万并发下，epoll_wait 返回的次数越少，服务器的吞吐量就越高。
```

当数据一次没有读取完，之前没读完的旧数据依然完好无损地保存在内核的接收缓冲区里，绝对不会丢失。旧数据和新数据会在缓冲区里按顺序排列（排队）。你接下来调用 recv 时，会先读出旧数据，再读出新数据。
- 会产生粘包问题
- 边缘触发下，会导致接收窗口关闭（TCP窗口死锁）
TCP内核接收缓冲区有大小限制，旧数据在缓冲区越积越多，会导致缓冲区被填满，服务器会向客户端宣告接收窗口为0，客户端无法发送新的数据，而由于是ET模式，没有新数据到达，不会再触发通知，epoll_wait收不到通知会死等下去，没有机会使用recv腾出缓冲区。
双方旧互相死等，直到连接废掉
- 水平触发避免了死锁风险，缓冲区变满，每次循环epoll_wait都会瞬间返回，不断提醒要读取缓冲区，epoll_wait会不停地执行recv，会慢慢消耗掉缓冲区存取的数据

区别就是：
```
水平触发（LT）与边缘触发（ET）在此处的本质区别
既然旧数据都在，那这两种模式在面对“没读完又有新数据”时，区别在哪？

水平触发（LT）：
新数据来之前：LT 就会因为缓冲区里有残余的 "FGH" 而不断疯狂通知你。
新数据来不来无所谓，只要里面有东西，LT 就会对你开启“夺命连环call”。你根本不需要等到新数据来，就能把旧数据读完。

优点：极度安全，你绝对不可能漏掉或卡死数据。

边缘触发（ET）：
新数据来之前：因为数据只是从 8 字节变成了 3 字节，状态没有“从无到有”或“从少变多”的发生突变，ET 绝对不会通知你。
直到阶段三（新数据 "12345" 到达的一瞬间）：缓冲区数据从 3 字节暴增到 8 字节，引发了“边缘变化”，ET 才会勉强通知你一次。如果你错过了这次机会（或者又没读干净），就真的可能再也读不到后面的数据了。
```



边缘触发模板
```c
// ET 模式下的标准写法
while (true) {
    int n = recv(fd, buf, sizeof(buf), 0);
    if (n > 0) {
        // 持续处理数据...
    } else if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // 数据终于被全部榨干了，安全退出循环
            break; 
        }
        // 处理其他真正的网络错误
        break;
    } else {
        // 对方关闭连接
        break;
    }
}
```


```c++
#include "common.h"
#include <iostream>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <unistd.h>

using namespace std;

static const int BACKLOG = 128;
static const int MAXEVENTS = 1024;

int tcp_listen(uint16_t port)
{
    // 1. 创建主动套接字
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket()");
        exit(-1);
    }
    // 2. 设置地址复用
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    // 3. 绑定套接字地址
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY); // 通配符地址
    int err = bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
    if (err) {
        perror("bind()");
        exit(-1);
    }
    // 4. 将主动套接字转变为被动套接字
    err = listen(sockfd, BACKLOG);
    if (err) {
        perror("listen()");
        exit(-1);
    }
    cout << "Server is listening..." << endl;
    return sockfd;
}

int main()
{
    // 1. 绑定通配符地址，监听8888端口
    int listenfd = tcp_listen(8888);
    // 2. 创建epoll实例（红黑树，双向链表）
    int epfd = epoll_create(1);
    if (epfd == -1) {
        perror("epoll_create()");
        exit(-1);
    }
    // 3. 将listenfd添加到epoll实例中（监听）
    struct epoll_event evt;
    evt.events = EPOLLIN;
    evt.data.fd = listenfd;
    int err = epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &evt);
    if (err) {
        perror("Add listenfd to epoll instance");
        exit(-1);
    }
    // 4. 事件循环
    for (;;) {
        struct epoll_event events[MAXEVENTS]; // 存放就绪事件
        // -1: 一直等待，直到有事件就绪或发生错误
        int nready = epoll_wait(epfd, events, MAXEVENTS, -1);
        if (nready < 0) {
            perror("epoll_wait()");
            break;
        }
        // 遍历所有已就绪的事件
        for (int i = 0; i < nready; ++i) {
            if ((events[i].events & EPOLLIN) == 0) {
                continue;
            }
            // a. listenfd就绪：有新连接 (事件1)
            if (events[i].data.fd == listenfd) {
                struct sockaddr_in cliaddr; // 用于接收客户端地址
                socklen_t length = sizeof(cliaddr);
				// 调用accept摘取新的连接，拿到用于传输数据的connfd
				
                int connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &length);
                if (connfd == -1) {
                    perror("accept()");
                    continue;
                }
                // 将connfd添加到epoll实例中（监听）
                evt.events = EPOLLIN;
                evt.data.fd = connfd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &evt);
                // 打印日志信息
                char ipstr[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &cliaddr.sin_addr, ipstr, INET_ADDRSTRLEN);
                uint16_t port = ntohs(cliaddr.sin_port);
                printf("新连接建立，客户端地址为 %s:%hu\n", ipstr, port);
            } else {
                // 可能客户端有数据到达，也可能是客户端断开了连接
                char buf[4096];
                int connfd = events[i].data.fd;
                int nbytes = recv(connfd, buf, sizeof(buf), 0);
                if (nbytes < 0) {
                    // 发生错误
                    perror("read()");
                    // 将connfd从epoll实例中删除（不再监听）
                    epoll_ctl(epfd, EPOLL_CTL_DEL, connfd, NULL);
                    close(connfd);
                } else if (nbytes == 0) {
                    // b. 客户端断开连接 (事件2)
                    // 将connfd从epoll实例中删除（不再监听）
                    epoll_ctl(epfd, EPOLL_CTL_DEL, connfd, NULL);
                    close(connfd);
                    printf("客户端断开连接，关闭文件描述符%d\n", connfd);
                } else {
                    // c. 客户端有消息到达（事件3）
                    for (int j = 0; j < nbytes; ++j) {
                        buf[j] = toupper(buf[j]); // 将字母转为大写
                    }
                    send(connfd, buf, nbytes, 0);
                    // d. 消息发送完毕 (事件3.5，这只能算半个事件)
                    printf("%.*s", nbytes, buf);
                }
            }
        }
    }

end:
    close(listenfd);
    return 0;
}

```

##### 一、 代码核心逻辑全梳理

这段代码实现了一个基于 **`epoll` 水平触发（LT）** 的高性能并发 TCP 回显（Echo）服务器。其生命周期清晰地分为 **初始化**、**事件监控注册** 和 **无限事件处理循环** 三大阶段：

```
                    +-----------------------------+
                    | 1. tcp_listen(): 创建、绑定 |
                    |   并开启 listenfd 的监听状态|
                    +--------------+--------------+
                                   |
                                   v
                    +-----------------------------+
                    | 2. epoll_create(): 在内核中 |
                    |    申请并创建 epoll 实例    |
                    +--------------+--------------+
                                   |
                                   v
                    +-----------------------------+
                    | 3. epoll_ctl(ADD): 将       |
                    |    listenfd 挂载到红黑树上  |
                    +--------------+--------------+
                                   |
                                   v
                    +-----------------------------+
+------------------>| 4. 无限循环：调用            |
|                   |    epoll_wait() 阻塞死等事件 |
|                   +--------------+--------------+
|                                  |
|                                  v (有事件就绪并返回 nready)
|                   +-----------------------------+
|                   | 5. for 循环：精准遍历从      |
|                   |    0 到 nready-1 的就绪事件  |
|                   +--------------+--------------+
|                                  |
|                                  v
|                       / 发生事件的 FD 是谁？ \
|                      /                        \
|                     <                          >
|                      \                        /
|                       \______________________/
|                        /                    \
|           (fd == listenfd)                 (fd == connfd)
|                  /                                    \
|                 v                                      v
|   +----------------------------+         +----------------------------+
|   | 6.1 新连接到达             |         | 6.2 客户端通信/异常事件    |
|   |   - accept() 抓取新连接     |         |   - 调用 recv() 读取数据   |
|   |   - epoll_ctl(ADD) 挂载    |         +--------------+-------------+
|   |     生成的 connfd 进树     |                        |
|   +--------------+-------------+                        v
|                  |                             / 检查 recv 返回值 \
|                  |                            /      nbytes      \
|                  |                           <                    >
|                  |                            \                  /
|                  |                             \________________/
|                  |                               /      |       \
|                  |                     (nbytes < 0) (nbytes==0) (nbytes > 0)
|                  |                           /          |          \
|                  |                          v           v           v
|                  |                    +-----------+ +-----------+ +-----------+
|                  |                    | 错误发生  | | 优雅断开  | | 业务回显  |
|                  |                    | -DEL 树   | | -DEL 树   | | -toupper  |
|                  |                    | -close()  | | -close()  | | -send()   |
|                  |                    +-----+-----+ +-----+-----+ +-----+-----+
|                  |                          |             |             |
+------------------+__________________________+_____________+_____________|
```

##### 二、 你的网络编程核心模糊点大起底

通过前面的层层深入，你刚才重点提问和理清的模糊点正是 Linux 高并发网络编程的灵魂所在。我们将其总结成以下四块“硬骨头”：

###### 1. 身份转变：主动套接字 vs 被动套接字

- **模糊点**：为什么非要用 `listen()`？
    
- **透彻掌握**：`socket()` 函数刚创建出来的 FD 默认是“主动”的（面向客户端，负责打出去）。服务端必须通过 `listen()` 强行将其扭转为“被动套接字”（面向服务端，负责守株待兔接听电话），并在内核中为其开辟**半连接队列**与**全连接队列**用于缓存三次握手成功的客户端。
    

###### 2. 安全规范：为什么要先 `EPOLL_CTL_DEL` 后 `close()`？

- **模糊点**：内核不是在 `close` 后会自动移除 `epoll` 里的 FD 吗？
    
- **透彻掌握**：只有当 FD 的系统引用计数归零时，内核才会自动清理。
    
    1. 如果在多进程或代码中 `dup()` 复制了描述符，仅调用 `close()` 无法清空红黑树，会导致“幽灵事件”**不断触发，瞬间**榨干 CPU 达到 100%。
        
    2. 在多线程高并发环境下，直接 `close()` 会释放当前数字，该数字极可能在千万分之一秒内被新连接复用，从而导致**其他线程误杀无辜的新连接**。因此，**先显式解绑监控，再销毁描述符**是黄金法则。
        

###### 3. 数据流底层：对方挂掉电话，我方 `recv` 为何返回 0？

- **模糊点**：怎么知道对方断开了？
    
- **透彻掌握**：在 TCP 四次挥手协议中，对方调用 `close()` 会发送 `FIN` 包。内核收到后，为了告知应用层代码“对方优雅关闭了发送通道，再也没有新数据了”，特意让 `recv()` 函数返回 **`0`**。在应用层，**`0` 就是网络半关闭/断开的专属信号**。
    

###### 4. 触发机制：水平触发（LT）与边缘触发（ET）

- **模糊点**：为什么一次没读完数据不会丢？ET 为什么高并发能力更强？
    
- **透彻掌握**：
    
    - **数据存储**：无论什么模式，没读完的旧数据都在内核缓冲区排队，新数据来了会自动追加在旧数据屁股后面，**数据绝对不会丢**。
        
    - **LT（水平触发，安全保底）**：只要缓冲区有剩数据，`epoll_wait` 就会开启夺命连环 Call，疯狂唤醒你，因此能天然免疫“接收窗口填满导致的死锁”风险。
        
    - **ET（边缘触发，极致性能）**：只在状态变化时通知一次，通知完就将 FD 移出就绪链表。它逼迫程序在应用层使用 `while(recv)` 循环配合**非阻塞 I/O** 榨干缓冲区。它之所以快，是因为**极大地减少了内核中就绪链表的维护成本，并成倍消减了高开销的 `epoll_wait` 系统调用次数**。



