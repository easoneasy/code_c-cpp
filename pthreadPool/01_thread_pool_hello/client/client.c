#include <my_header.h>

int main(int argc, char *argv[])
{
    char *ip = "192.168.100.128";
    char *port = "12345";

    //创建文件描述符
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    ERROR_CHECK(listen_fd, -1, "socket");

    //绑定服务器的ip与端口号
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));//初始化addr

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);//本机字节序转换为网络字节序
    addr.sin_port = htons(atoi(port));

    //调用connect函数进行三次握手
    int ret = connect(listen_fd, (struct sockaddr *)&addr, sizeof(addr));
    ERROR_CHECK(ret, -1, "connect");
    //-----------------------------走到这里就是客户端的socket的代码----
    //-------走到这里就说明三次握手已经建立成功了，同时可以与服务器
    //传输数据

    //接收服务器发送过来的数据
    char buf[100] = {0};
    int cnt = recv(listen_fd, buf, sizeof(buf), 0);
    printf("recv cnt: %d\n", cnt);
    printf("client buf : %s\n", buf);

    close(listen_fd);
    return 0;
}

