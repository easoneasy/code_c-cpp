#include "socket.h"
#include <my_header.h>

void init_socket(int *fd, char *ip, char *port)
{
    //创建用于监听的文件描述符
    *fd = socket(AF_INET, SOCK_STREAM, 0);
    ERROR_CHECK(*fd, -1, "socket");

    //设置端口复用
    int opt = 1;
    setsockopt(*fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    //绑定服务器的ip与端口号
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(port));
    addr.sin_addr.s_addr = inet_addr(ip);

    int ret = bind(*fd, (struct sockaddr *)&addr, sizeof(addr));
    ERROR_CHECK(ret, -1, "bind");

    //监听客户端，（填充全连接队列与半连接队列）
    ret = listen(*fd, 100);
    ERROR_CHECK(ret, -1, "listen");
}
