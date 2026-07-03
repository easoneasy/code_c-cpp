

1、启动Nginx容器
```
// 后台启动
docker run -d nginx  
// 查看正在运行的端口
docekr ps
// tcp 和 udp 的端口，是不是同一个端口

// 查看所有容器
docker ps -a


// 删除,不能删除一个正在运行的容器，需要先stop
docker rm + name/id

// 强制删除
docker rm -f +name/id 

// 在后台启动+赋值新名字为app1
docker run -d --name app1 nginx

```

2、修改Nginx的首页
```
// 交互式
// 让app1这个容器执行bash命令，就进入容器里
docker exec -it app1 bash 
// 镜像本质是文件系统，存放文件

/usr/share/nginx/html
// 无法被外界访问，需要进行端口映射
// 容器中的端口是和虚拟机交互的80端口，而不是虚拟机与外部交互的80端口
// 让nginx容器的80端口与虚拟机的80端口映射，可以在外部访问nginx
docker run -d --name app1 -p 80:80 nginx

// 修改了Nginx的首页
echo "hello" > /usr/share/nginx/html/index.html
```

3、将修改后的容器保存为镜像
```
docker commit 
// 将容器保存为镜像
docker commit -m "change index.html" app1 mgnginx:v1.0.0
// 查看镜像列表
// 放在自己的docker引擎里，可以通过docker push保存到官方仓库
docker images
// 导出成tar包
docker save
docker save -o mgnginx.tar mgnginx:v1.0.0
```

6、从tar包导入镜像
```
// 从tar包中导入
docker image load -i mgnginx.tar
 
```


##### 挂载

1、绑定挂载
```
// 在容器和宿主机之间实现绑定挂载和卷挂载
// 实现数据持久化和文件共享
docker run -v

// www目录和nginx的/usr/share/nginx/thml 挂载，是同一个内容
// 在www目录下修改内容，在nginx的目录里也能看到，实现绑定挂载
将数据存放在宿主机内，容器只是引用目录
把容器删掉，宿主机内的数据不会丢失
后面重新启动容器可以实现同样的绑定挂载，内容不会改变
docker run -d --name app1 -p 80:80 -v ./www:/usr/share/nginx/html nginx

// problem：需要记住哪个数据存放在宿主机的哪个目录
```

2、卷挂载
卷是docker管理的特殊目录，在容器之间共享和持久化数据，卷的数据存放在宿主机上
```
docker volume 

docker volume create volume1

docker volume inspect volume1
[
    {
        "CreatedAt": "2026-06-10T11:22:12+08:00",
        "Driver": "local",
        "Labels": null,
        "Mountpoint": "/var/lib/docker/volumes/volume1/_data",
        "Name": "volume1",
        "Options": null,
        "Scope": "local"
    }
]

Mountpoint是挂载点，存放在宿主机内

```

##### 自定义网络
Docker 为每个容器都分配了唯一的 ip 地址，容器使用 ip 地址和端口，可以相互访问

```
docker inspect 

docker network

1容器的端口是80
2容器的端口是88
从1->2
curl 192.168.254.128:88
需要从宿主机出去再通过宿主机的88端口访问容器2

curl 容器2的内网ip
在宿主机内访问容器2

// 使用容器名通信

// 创建网路network1
docker network create network1

// 启动容器1，放入network1
docker run -d --name app2 -p 88:80 --network network1 nginx
// 启动容器2，放入network1

// 进入容器app1可以通过容器名访问app2
```