mmp复制大文件，length和offset
重定向dup，交替更改文件标识符的指向

Day20作业题
1.采用mkdir 的方案，批量创建1000个文件夹。  
最外层的路径，由命令行参数给出。  
里层的名字分别为 test1 test2 test3 .... test1000
在test1中，分别也创建 test11 test12 test20  
在test2中，创建 test20 test21 test22 ... test30  
依次类推。 感受一下，为什么需要用程序来做这些事情。
```c
#include <my_header.h>
int main(int argc,char *argv[])
{
    mode_t mode =0777;
    if(argc==3){
        sscanf(argv[2],"%o",&mode);
    }
    char father_path[100]={0};
    for(int i=1;i<=1000;i++){
	    //father_path 存放的是argv[1]/testi
        sprintf(father_path,"%s/test%d",argv[1],i);
        int ret1=mkdir(father_path,mode);
        ERROR_CHECK(ret1,-1,"mkdir father_path failed.\n");
        chdir(father_path);

        char children_path[100]={0};
        for(int j=1;j<=10;j++){
            sprintf(children_path,"test%d",i*10+j);
            int ret2=mkdir(children_path,mode);
            ERROR_CHECK(ret2,-1,"mkdir children_path failed.\n");
        }
        //进入了argv[1]里，但是实际上要进入argv[1]的上一级
        /* chdir(argv[1]); */
        
        chdir("../..");
    }
    return 0;
}

```

内存映射
直接将文件的内容映射到内存中，读内存相当于读文件，改内存相当于改文件

###### mmp映射
```c
void *mmap(void *addr,size_t length,int prot,int flags,int fd,off_t offset);
映射失败返回MAP_FAILED(是一个宏)

addr  直接写NULL(系统自己找空间存放)
//length 和 offset 定位映射哪一段
int prot 权限 r  /  r|w  两种权限
int flags 映射的内存的回写特点：map_shared
int fd 是打开文件

char *p = mmap(NULL,10,PROT_READ,MAP_SHARED,fd,0);
只会存前10个字节
for(int i=0;i<10;i++){
	printf("%c\n",p);
}
```

length 和 offset 定位映射的问题（容易踩坑）
length是长度
offset的值只能是4096的倍数，如果是其他值会出现总线错误。
offset ：这一次要从文件的哪个位置开始读

如果映射的数目大于文件本身的大小也可能会出现总线错误。Bus error -->访问到了不该访问的内存
###### munmap解除映射
映射多少就解映射多少
```c
munmap(p,10);
```

###### mmap复制大文件
使用分段映射，再将每一段弄出去
1、文件大小怎么拿  fstat
```c
int fd = open(argv[1],O_RDONLY);
struct stat stat_buff;
fstat(fd,&stat_buff);
printf("%ld\n",stat_buf.st_size);
```
2、offset的值只能是**4096**的倍数，需要考虑要复制的文件大小如果不是4096的倍数，剩下的文件数据应该怎么取？
3、memcpy，把src的数据复制到dest中

```c
#include <my_header.h>
#define ONCE_MAX_COPY_SIZE (1024 * 1024 * 16)

int main(int argc, char *argv[]){

    int src_fd = open(argv[1], O_RDONLY);
    int dest_fd = open(argv[2], O_RDWR | O_CREAT |O_TRUNC, 0666);
    ERROR_CHECK(src_fd, -1, "open src failed");
    ERROR_CHECK(dest_fd, -1, "open dest failed");

    struct stat stat_buf;
    fstat(src_fd, &stat_buf);
	//源文件的大小：
    long src_total_size = stat_buf.st_size;
    ftruncate(dest_fd, src_total_size);
	//已经复制的文件大小
    long copied_size = 0;

    while(copied_size < src_total_size){
    //当前复制的数据的长度，如果比16M大就用16M，如果比16M小就使用剩下的长度作为复制长度
        int cur_copy_size = (src_total_size - copied_size) > ONCE_MAX_COPY_SIZE ? ONCE_MAX_COPY_SIZE:
            (src_total_size - copied_size);
		
		//copied_size是上一次复制数据的位置，已经拷到哪了，下一次还从这里开始
        // 这把要拷贝的大小。  + copied_size --->offset 
        //cur_copy_size就是一次拷贝的长度16M，如果是剩下的不足16M，就直接拷贝
        //是实际拷贝的长度
        char *src_p = (char *)mmap(NULL, cur_copy_size, PROT_READ, MAP_SHARED, src_fd, copied_size);
        ERROR_CHECK(src_p, MAP_FAILED, "mmap src failed");
        
        char *dest_p = (char *)mmap(NULL, cur_copy_size, PROT_READ | PROT_WRITE, MAP_SHARED, dest_fd, copied_size);
        ERROR_CHECK(dest_p, MAP_FAILED, "mmap dest failed");

        // 现在，两个内存映射进来了。 
        // for(i=0;i<cur_copy_size; i++){dest_p[i] = src_p[i];}
		//memcpy函数：把src的数据复制到dest中 
        memcpy(dest_p, src_p, cur_copy_size);
        munmap(src_p, cur_copy_size);
        munmap(dest_p, cur_copy_size);

        copied_size += cur_copy_size;
    }
    close(src_fd);
    close(dest_fd);

    return 0;
}
```


mmap相当于，让内核缓冲区对应的物理内存，在用户空间也有一块虚拟内存对应到这个物理内存
==让用户空间的虚拟内存  和  内核的虚拟内存     映射    到同一块物理内存==


读普通文件，不在乎效率：fread、read（数组要大一点）  都行
Linux开发中：read、write出现得更多
大文件的复制：mmap
理论上：read比fread少一次数据复制，mmap比read少一次数据


##### 重定向
程序在启动的时候，系统会自动开三个流：stdin、stdout、stderr
它们会分别占据3个文件描述符。                    0       1(行缓冲)    2 
STDIN_FILENO      0
STDOUT_FILENO  1
STDERR_FILENO   2 
 ```c
 printf("111");  //有缓冲区的输出，存在缓冲区中
 write(1,"222",3);   //无缓冲区，直接输出到文件标识符为1的文件中（屏幕）
 --> 222111  
 ```

有缓冲区文件流是满缓冲区，缓冲区满才会输出到内核
```c
    FILE *fp = fopen("222.txt","w");
    ERROR_CHECK(fp,NULL,"fp open failed.\n");
    fputs("hello\n",fp);
    write(3,"world",5);
    fclose(fp);
    -->worldhello
    //文件是满缓冲区，遇到换行符不会刷新
    //write直接写到fp内，所以先输入world
    //而文件关闭的时候才会刷新,这时才会把hello写入文件
```
fflush：手动刷新缓冲区
```c
FILE *fp = fopen("222.txt","w");
    ERROR_CHECK(fp,NULL,"fp open failed.\n");
    fputs("hello",fp);
    fflush(fp);  //刷新缓冲区
    write(3,"world",5);
    fclose(fp);
```
printf是标准输出，是一个行缓冲区的输出
每次都往文件描述符为1这里放数据
行缓冲区遇到 \n 会刷新
```c
	//遇到\n行缓冲区刷新，向屏幕输出
    printf("hello\n");
    //直接向行缓冲区输出6个数据
    fwrite("world\n",1,6,stdout);
```

fopen在底层使用的是open（用到了文件描述符）。  
有满缓冲区,满了才会刷新，要写入文件内的内容会先放在缓冲区
write直接先到文件对象中，关闭之前缓冲区会刷新，将fopen的内容输出到文件对象

==1、printf只往文件描述符为1的这里输入==
==2、close（1）只是关文件描述符到文件对象之间的映射关系（屏幕也是文件）==
==3、系统在初始创建的三个文件对象，到最后才会关闭，可以手动关闭==

系统分配文件描述符是**从小到大**的顺序，如果一个文件描述符没有人用，就会分配给别人再次利用
```c
printf("hello wolrd\n");
    close(1);
    int fd=open("222.txt",O_WRONLY);
    //printf直接向文件输入
    //printf只向文件标识符为1的地方输入，已经将1指向的文件（屏幕）释放掉，现在1分配给了fp指向的文件。
    printf("fd = %d\n",fd);
    close(fd);
```

###### dup
申请一个新的文件描述符，指向同一个文件对象
```c
    int fd = open("222.txt",O_WRONLY|O_TRUNC);
    write(fd,"12345",5);
	//使用dup新建一个文件标识符，指向和fd的同一个文件对象
    int new_fd=dup(fd);
    //都会写入222.txt
    write(fd,"67890",5);
    write(new_fd,"abcde",5);
    printf("fd = %d\n",fd);
    printf("new_fd=%d\n",new_fd);
	// --> fd = new_fd
    close(fd);
    close(new_fd);
```
交替向屏幕、文件中输出内容，只能用printf（使用dup备份）
```c
    printf("---------\n");
    //此时fd的文件标识符是3，指向文件1.txt
    int fd = open("1.txt", O_WRONLY | O_TRUNC);
    ERROR_CHECK(fd, -1, "open failed");
    
    printf("11111\n");
	//stdout_backup_fd新的文件标识符4，指向屏幕
    int stdout_backup_fd = dup(STDOUT_FILENO);
    //关闭了文件标识符为1的指向
    close(STDOUT_FILENO);
    //获得了1的指向，指向1.txt文件
    //向1.txt输出
    dup(fd);
    printf("22222\n");
	//关闭标识符为1的指向
    close(STDOUT_FILENO);
    //获得了1的指向，指向屏幕
    //向屏幕输出
    dup(stdout_backup_fd);
    printf("33333\n");
	//关闭了1的指向
    close(STDOUT_FILENO);
    //获得了1的指向，指向1.txt文件
    dup(fd);
    printf("44444\n");
    //关闭了1的指向
    close(STDOUT_FILENO);
    //获得了1的指向，指向屏幕
    dup(stdout_backup_fd);
    printf("55555\n");
	
    close(fd);
    close(stdout_backup_fd);
    close(STDOUT_FILENO);
    //最终，1、3、5输出屏幕
    //2、4输入文件内
```
![[dup重定向.png]]

###### dup2
向系统索要指定文件描述符
如果文件描述符没有被使用，直接给
如果文件描述符正在被使用，将文件描述符和文件对象之间的关系断掉再给
返回值就是系统告诉成功给/不给 
// 因为在第一次使用输出的时候。 缓冲区行为和终端行为保持一致。 
// 如果第一次输出，指向的是终端。就是一个行缓冲区
// 如果第一次输出，指向的是文件，是满缓冲区。
```c
    printf("---------------\n");
    //备份1的指向，指向屏幕
    int stdout_backup_fd = dup(STDOUT_FILENO);
    //新建了3文件描述符，指向1.txt
    int file_fd = open("1.txt", O_WRONLY |O_TRUNC);
	//将1的指向给文件，就是将1指向1.txt
    dup2(file_fd, STDOUT_FILENO);
    printf("11111.\n");
    //将1指向屏幕
    dup2(stdout_backup_fd, STDOUT_FILENO);
    printf("22222.\n");
    //将1指向文件
    dup2(file_fd, STDOUT_FILENO);
    printf("33333.\n");
    //将1指向屏幕
    dup2(stdout_backup_fd, STDOUT_FILENO);
    printf("44444.\n");
	
    close(stdout_backup_fd);
    close(file_fd);
    close(STDOUT_FILENO);
```

输入重定向
```c
    // scanf ： 本来从键盘进。 从文件描述符0读数据。 
    // 如果将0关掉，重新打开了一个文件。 占用文件描述符0
    // 使用scanf 直接从文件读数据。 
    close(STDIN_FILENO);

    ARGS_CHECK(argc, 2);
    int fd = open(argv[1], O_RDONLY);
    printf("fd = %d\n", fd);

    int num1 = 0;
    double d1 = 0.0;
    scanf("%d %lf", &num1, &d1);
    
    printf("num1 = %d\n d1 = %lf\n", num1, d1);
```