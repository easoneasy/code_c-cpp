#include "send_file.h"
#include <my_header.h>

void send_file(int fd)
{
    send(fd, "hello", 5, 0);
}
