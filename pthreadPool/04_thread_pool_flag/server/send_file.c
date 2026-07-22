#include "send_file.h"
#include <my_header.h>

void send_file(int fd)
{
    char *file_name = "The_Holy_Bible.txt";
    /* send(fd, "hello", 5, 0); */
    //将文件名的长度  + 文件名 + 文件内容(文件长度)
    int file_fd = open(file_name, O_RDWR);

    struct stat st;
    fstat(file_fd, &st);
    off_t file_len = st.st_size;//获取文件内容的长度


    //这里还要注意：如果对一个已经断开的文件描述符执行两次以及以上的
    //写操作会触发SIGPIPE信号，导致程序崩溃，为了解决这个问题，
    //可以将send的第四个参数设置为MSG_NOSIGNAL
    int len = strlen(file_name);//18
    send(fd, &len, sizeof(int), MSG_NOSIGNAL);//文件名的长度
    send(fd, file_name, len, MSG_NOSIGNAL);//file_name指向的文件的长度18字节

    send(fd, &file_len, sizeof(off_t), MSG_NOSIGNAL);

    //零拷贝操作，数据在内核之间进行拷贝
    sendfile(fd, file_fd, NULL, file_len);

    close(file_fd);
}
