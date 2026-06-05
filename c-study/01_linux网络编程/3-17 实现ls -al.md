打开目录流的标准操作流程，使用的dirent结构体获取name
使用stat结构体可以获取ls -al相同的内容
扩展：时间戳打散-->时间，文件类型+权限拆开，权限&问题
按照文件名排序

chdir 切换工作路径
每个程序都有自己的工作路径，工作路径是继承的，是相对路径的起点
//相对路径受工作目录的控制

getcwd 确认工作路径的位置
```c
getcwd(装路径的数组，数组的大小sizeof(数组));
//打印路径直接输出数组的内容
```

mkdir 创建目录
//会受到umask掩码的限制。权限可写可不写
//777-->775
mkdir的返回值是int类型，mode也是int。argv[]是字符串，需转换sscanf
且mode是8进制，使用%o. 
权限可写可不写，使用if语句判断argc的值
```c
mkdir("路径"，权限);
//返回值：int类型，-1失败  perror()查看错误信息
```
xxx_t 是类型别名
```c
mode_t mode =0777;   //8进制的0777
if(argc==3){
	sscanf(argv[2],"%o",&mode);
}
int ret=mkdir(argv[1],mode);
ERROR_CHECK(ret,-1,"mkdir failed."\n);
```

rmdir 删除空目录
//返回值是int类型
```c
rmidr(要删除目录的路径（相对/绝对）);
```


##### 目录流
 目录中存放目录下有什么内容，也就是它的子项
 cpp71 目录下装了 day1 day2 day3 test1.c test2.c
不断地读，会一个一个拿回来，是目录项 struct dirent，直到读完
返回特殊值
只能读目录，不可以写目录（会破坏它的结构）

文件流标准操作：
1、打开文件流
```c
FIEL *fp =fopen("1.txt","r");
```
2、判断是否打开成功（NULL）
```c
ERROR_CHECK(fp,NULL,"fopen failed.\n");
```
3、使用
```c
int ch;
while(ch=fgetc(fp)!=EOF){
	printf("%c\n",ch);
}
```
4、关闭文件流
```c
fclose(fp);
```

目录流的标准操作
```c
//打开
DIR *dirp =opendir("路径");
ERROR_CHECK(dirp,NULL,"opendir failed.\n");
//使用
struct dirent *dirent_p=NULL;
//dirent_p指向了NULL，是因为readdir函数内部自己分配空间，返回一个指针，
while((dirent_p=readdir(dirp))!=NULL){
	printf("",dirent_p->d_ino);
}
//关闭
closedir(dirp);
```
dirent结构体中 需要记忆的：d_ino、d_name、d_type


##### stat系统调用
获取一个文件/文件夹的详细信息
包括ls -al的所有输出，除了文件的名字
```
stat(路径，指向有效内存的指针)
```

```c
struct stat *stat_p=NULL;
stat("1.txt",stat_p);
//指针指向了NULL，不是有效内存
//stat函数自己不会分配内存，需要手动分配内存，从而存储要传出的数据
struct stat stat_buf;
stat("1.txt",&stat_buf);
//指针指向了栈内存，是有效内存
int ret=stat("1.txt",stat_p);
ERROE_CHECK(ret,-1,"stat failed.\n");

stat结构体中的主要对应ls -al的内容：
stat_p->st_mode //文件类型+权限
stat_p->st_nlink //硬链接数
stat_p->st_uid   //文件所有者的用户id
stat_p->st_gid   //组id
stat_p->st_size  //文件大小
stat_p->st_mtim  //最后一次修改的时间 //是个结构体，装时间戳
```

```c
//struct stat *tsat_p=malloc(sizeof(struct stat));
//左右类型指针类型不一样，有风险，需要强制性类型转换
struct stat *tsat_p=(stat stat *)malloc(sizeof(struct stat));
int ret=stat(argv[1],stat_p);
ERROR_CHECK

printf("%o %d")
//不确定类型，先%d
//vim报错的时候会显示哪里错了，到时候再改
```

###### 使用目录流获取目录中的子项
可以提供inode 、type、name （主要）
```c
ARGC_CHECK
DIR *dirp = opendir(argv[1]);
ERROR_CHECK

struct dirent *dirent_p=NULL;
struct stat *stat_p =calloc(1,sizeof(struct stat));
while (dirent_p = readdir(sirp)!=NULL){
	//打印文件中的文件名称
	printf("%s\n",dirent_p->d_name);
	//可以将readdir获取的name，塞到stat中，获取详细信息
	//但是这是相对路径
	//解决方法1：可以切换当前的工作路径 chdir
	           //chdir(argv[1]);
	//==解决方法==2：将路径和readdir获得的name拼接在一起
	//sprintf
	//char path[100] = {0};
	//sprintf(path, "%s/%s", argv[1], dirent_p->d_name);
    //int ret = stat(path, stat_p);  
    //stat第一个参数需要路径，所以将命令行输入的路径参数与要输出的文件名输入到字符串中保存
    
    
	stat (dirent_p->d_name,stat_p);
}

closedir(dirp);
```

##### 简单的ls -al的实现：
```c
#include <my_header.h>
int main(int argc,char *argv[])
{
    //使用目录流，获取目录中的子项(drient.name)
    ARGS_CHECK(argc,2);
    DIR *dirp = opendir(argv[1]);
    ERROR_CHECK(argv[1],NULL,"opendir failed.\n");
	//使用这个指针指向readdir返回的目录项
    struct dirent *dirent_p =NULL;
    //申请stat结构体的空间,申请的空间需要强制类型转换成结构体指针类型
    struct stat *stat_p =(struct stat*)calloc(1,sizeof(struct stat));
    //将获取到的dirent_p的name放入stat中
    //readdir是从目录流中一个一个读取目录项，返回struct dirent的指针
    while((dirent_p=readdir(dirp))!=NULL){
        int ret = stat(dirent_p->d_name,stat_p);
        ERROR_CHECK(ret,-1,"stat failed.\n");
        
        printf("%o %ld %d %d %ld %ld %s\n",stat_p->st_mode,
               stat_p->st_nlink,stat_p->st_uid,stat_p->st_gid,
               stat_p->st_size,stat_p->st_mtim.tv_sec,dirent_p->d_name);
    }
    closedir(dirp);
    return 0;
}

```

将文件类型+权限-->文本类型
权限：
mode &0400 ==> mode & 100 000 000
```c
// 0400是八进制
file_type_and_mode[1] = (((mode &0400))!=0) ? 'r':'-';
```

u_id 、g_id ---> name
```c
getpwuid(uid)->pw_name
getgrgid(uid)->gr_name
```

时间戳的转换，需要使用到localtime -->年月日、时分秒
```
localtime(数组)
//需要一个数组存放打散的时间 
时间戳是整型，变成时间就要转换成字符串形式
将整型-->字符串 使用sprintf(存储字符串的数组,"%s",字符串);
```

##### ls -al的代码实现
```c
#include <my_header.h>
//mode-->文件类型+权限表示
void parse_file_type_and_mode(mode_t mode,char *type_and_mode){
    char file_type = '?';
    switch (mode & S_IFMT) {
    case S_IFBLK:  file_type='b';            	break;
    case S_IFCHR:  file_type='c';        		break;
    case S_IFDIR:  file_type='d';               break;
    case S_IFIFO:  file_type='p';               break;
    case S_IFLNK:  file_type='l';               break;
    case S_IFREG:  file_type='-';            	break;
    case S_IFSOCK: file_type='s';               break;
    default:       file_type='?';               break;
    }

    type_and_mode[0] = file_type;
    //权限  mode & 0400 0200 0100   0040 0004
    //mode & 100 000 000 
    //mode是8进制,需转成2进制
    //664  的读权限
    //6--->110 000 000 & 100 000 000
    //6--->000 110 000 & 000 100 000
    //4--->000 000 100 & 000 000 100
    type_and_mode[1] = ((mode & 0400)!= 0 ) ? 'r':'-';
    type_and_mode[2] = ((mode & 0200)!= 0 ) ? 'w':'-';
    type_and_mode[3] = ((mode & 0100)!= 0 ) ? 'x':'-';
    type_and_mode[4] = ((mode & 0040)!= 0 ) ? 'r':'-';
    type_and_mode[5] = ((mode & 0020)!= 0 ) ? 'w':'-';
    type_and_mode[6] = ((mode & 0010)!= 0 ) ? 'x':'-';
    type_and_mode[7] = ((mode & 0004)!= 0 ) ? 'r':'-';
    type_and_mode[8] = ((mode & 0002)!= 0 ) ? 'w':'-';
    type_and_mode[9] = ((mode & 0001)!= 0 ) ? 'x':'-';
    type_and_mode[10] = '\0';
}

//将时间戳转成字符串形式输出
void parse_time_to_str(time_t sec,char *str){
    struct tm *tm_ret=localtime(&sec);
    //将时间戳打散，输出到str中
    sprintf(str,"%d-%02d-%02d %02d:%02d:%02d",
            tm_ret->tm_year+1900,tm_ret->tm_mon+1,tm_ret->tm_mday,
            tm_ret->tm_hour,tm_ret->tm_min,tm_ret->tm_sec);
}

int main(int argc,char *argv[])
{
    ARGS_CHECK(argc,2);
    DIR *dirp=opendir(argv[1]);
    ERROR_CHECK(dirp,NULL,"opendir failed.\n");

    struct dirent *dirent_p = NULL;
    struct stat *stat_p=(struct stat*)calloc(1,sizeof(struct stat));
    while((dirent_p=readdir(dirp))!=NULL){
        int ret = stat(dirent_p->d_name,stat_p);
        ERROR_CHECK(ret,-1,"stat failed.\n");
        //将mode转换成字符串形式
        char type_and_mode[20]={0};
        parse_file_type_and_mode(stat_p->st_mode,type_and_mode);

        //将时间戳转成字符串
        char time_str[200]={0};
        parse_time_to_str(stat_p->st_mtim.tv_sec ,time_str);

        printf("%s %ld %s %s %ld %s %s\n",type_and_mode,stat_p->st_nlink,
               getpwuid(stat_p->st_uid)->pw_name,
               getgrgid(stat_p->st_gid)->gr_name,
               stat_p->st_size,time_str,dirent_p->d_name);
    }
    return 0;
}

```



按照文件名排序
打印tree的结果
```c
if mode == 32
```