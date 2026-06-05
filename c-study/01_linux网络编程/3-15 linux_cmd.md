
xxd  是以二进制的形式打开


把.c文件-->可执行文件
```
gcc test1.c -o test1
```

一次执行两条命令
```
cmd1 ; cmd2
mkdir test1 ;cd test1   //创建test1目录；再进入test1目录内
```

将前一个命令的输出，作为后一个命令的输入
```
cmd1 | cmd2
ll | grep -name "test"  //查看当前目录的内容|将输出的内容作为grep以文件名的方式找文件内容
```

查看文件内的行数
```
wc -l test1.txt
```

目录权限
```
目录一定要有x权限

如果目录无w权限，那么可以写目录中文件里的内容
				但不可以写目录中的文件（重命名、移动、新建、删除）
```

文件的权限掩码
```
权限= 默认权限 & ~umask   //~umask取反
umask 默认为0002
others如果默认有写权限，则最终的权限是减掉others的写权限,如果没有则不用减
eg： 777  --->   775  //去掉others的写权限
eg： 775  --->   775
```

修改c语言模板   位置
```
 vim ~/.vim/plugged/prepare-code/snippet/snippet.c
```




GNU
预处理：
```
gcc -E hello.c -o hello.i
```

编译-->生成汇编代码
```
gcc -S hello.i/hello.c  -o hello.s
```

汇编-->生成机器指令（二进制代码）
```
gcc -c hello.c/.i/.o -o hello.o

使用xxd命令查看二进制代码的内容
或vim 中可以使用:%!xxd  可以查看
```

链接
```
gcc hello.o -o hello
```

链接两个.c ＋一个.h 文件，生成一个hello可执行文件
```
gcc hello.c hello1.c -o hello
```

```
-wall  打印所有的警告信息
-O0  debug时不优化
-g   调试的时候使用，保留变量名
```

GDB调试程序
```
gcc hello.c -o test1 -wall -g -O0
```

创建公共头文件
//在.h文件内引入头文件
//使用时直接#include<my_header.h>
```
vim /usr/include/my_header.h
```

更改.c文件模板
//修改文件内容即可
```
vim ~/.vim/plugged/prepare-code/snippet/snippet.c
```

makefile
在~/tools下新建makefile文件，使用时直接复制到当前文件夹下使用即可，只会编译最近更新过的.c文件
之前没动的.c文件不会重新编译
//相当于gcc test1.c -o test1 -g -Wall -O0
```
SRCS := $(wildcard *.c)
OUTS := $(patsubst %.c,%,$(SRCS))
CC := gcc
COM_OP := -Wall -g 
.PHONY: clean rebuild all

all: $(OUTS)                      
% : %.c
    $(CC) $^ -o $@ $(COM_OP)

clean:
    $(RM) $(OUTS)

rebuild: clean all
```


helloworld（文件复制，将判断语句简写，使代码更简洁）

公共头文件需要加
直接在.c文件中根据写的格式使用
```
#ifndef MY_HEADER_H
#define MY_HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 检查命令行参数数量是否符合预期
#define ARGS_CHECK(argc, expected) \
    do { \
        if ((argc) != (expected)) { \
            fprintf(stderr, "args num error!\n"); \ 
            exit(1); \       
        } \                          
    } while (0)                      

// 检查返回值是否是错误标记,若是则打印msg和错误信息
#define ERROR_CHECK(ret, error_flag, msg) \
    do { \
        if ((ret) == (error_flag)) { \
            perror(msg); \
            exit(1); \
        } \
    } while (0)

#endif
```
do-while(0)
是为了防止写if语句时不加大括号出现的错误

```
#include <my_header.h>

// 将src文件复制到目标dest,若dest存在则直接覆盖
void copy_file(const char *src, const char *dest) {

    FILE *src_fp = fopen(src, "rb");
    // 对返回值进行错误处理
    ERROR_CHECK(src_fp, NULL, "fopen src");

    FILE *dest_fp = fopen(dest, "wb");
    %% if (dest_fp == NULL) {
        perror("fopen dest");
        fclose(src_fp);  // 关闭已打开的源文件
        exit(1);
    } %%
    ERROR_CHECK(dest_fp, NULL, "fopen dest");
    //可以不关闭src文件，因为头文件中的写法是exit，会退出程序

    // 用于临时存储数据的中转站
    char buf[1024] = { 0 }; 
    size_t count;

    // 从源文件读取数据并写入目标文件
    while ((count = fread(buf, 1, sizeof(buf), src_fp)) > 0) {
        fwrite(buf, 1, count, dest_fp);
    }

    // 关闭文件流
    fclose(src_fp);
    fclose(dest_fp);
}

int main(int argc, char *argv[]) {
    ARGS_CHECK(argc, 3);
    copy_file(argv[1], argv[2]);
    return 0;
}
```


