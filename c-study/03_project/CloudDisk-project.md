
## V1
### 注册
##### 1、首先判断客户端的发送格式是不是json
```c++
req->content_type() == APPLICATION_JSON
```

注册是POST方法，要获取request的请求体，提取username、password、confirm，
```c++
// 前端发送的是Content-Type:application/json
// 使用 req->json()格式获取json内容
// 类似于
// string body = req->body();
// json data = json::parse(body);
// 加引用是因为防止拷贝（大文件）会占用资源
auto &data = req->json();
```

```
接口      	Content-Type	                      获取数据
注册 	    application/json	                req->json()
登录 	    application/json	                req->json()
上传文件	    multipart/form-data	                req->form()
老式HTML表单	application/x-www-form-urlencoded	req->form_kv()
```

##### 2、将密码转成哈希值
生成随机盐值
将密码+随机盐值-->哈希值

将随机盐值、哈希值放入数据库中

##### 3、将用户信息存入数据库，数据库函数
拼好sql语句
```
将username、password、salt放入数据库中
```
执行MySQL异步查询+回调处理结果
```
// SQL执行结果
MySQLResultCursor *cursor

先判断SQL是否执行成功
MYSQL_STATUS_OK
// 执行sql语句影响的行数
resp->get_affected_rows()
```

### 登录

##### 1、判断客户端发送格式

##### 2、从request中获取用户名、密码

##### 3、通过用户名查找 hashcode、salt
将查到的salt与password哈希  再与查到的hashcode对比
相同-->返回token（CryptoUtil::generate_token会生成）
不相同--> 返回错误


### 查询用户信息

##### 1、验证身份 + 令牌类型
```
获取头部的Authorization的值
// 需要加const、&
// & ： 复制的代价很大，最好使用引用
// 加const：req内部保存的请求头不希望被修改

const string &auth = req->header("Authorization");

也可以写成
const auto &auth = req->header("Authorization");
```

##### 2、检查是否有令牌或令牌是否合法

```
auth.size() < 8 || auth.substr(0,7) != "Bearer";
```

##### 3、获取token
直接从头部截取

##### 4、创建user对象

user是一个类，里面存放用户的信息
通过解析token，将用户的信息放入结构体中
```
CryptoUtil::verify_token(token, user)
```
上述代码执行完成之后，就会将token中携带的信息放入对象user中

##### 5、将用户信息返回给浏览器
使用json格式返回

### 查询文件列表

##### 1、身份验证 + token认证
可以从token中获取用户的信息  user对象的所有信息

##### 2、通过用户uid从数据库中获取文件信息

拼接sql语句
执行MySQL异步查询+回调处理结果

json的格式
```
{
    "files":[
        {
            "fileId":1,
            "filename":"a.txt",
            "size":100
        },
        {
            "fileId":2,
            "filename":"b.txt",
            "size":200
        }
    ]
}
```

是 json数组
```
json arr = json::array();
添加数据
arr.push_back();
```

在while循环中创建一个json对象f
将f的内容填充完成之后push_back到json数组中


### 上传文件
##### 1、先检查上传文件的格式是否为form格式
文件上传只能用 `multipart/form-data`

```
file
  ├─ first
  │   test.txt   // 文件名
  │
  └─ second
      hello world  // 文件内容
```

##### 2、验证身份 + token认证
从token中获取uid

##### 5、解析文件

```
const Form &form = req->form();

与

auto file = form.at("file");
```

```
const Form &form = req->form();
拿到的是整个表单
将multipart/form-data 数据解析成form对象
form
{
    "file" :
    {
        "test.txt",
        "hello world"
    }
}
可以理解成：
map<
    string,
    pair<string,string>
>
```

```
auto file = form.at("file");
获取的是具体的pair对象 --> 具体的file字段
--->
"file" :
    {
        "test.txt",
        "hello world"
    }
pair<string,string>
filename = file.first
filedata = file.second
```



### v2
使用阿里云服务器备份 OSS

由于网络资源初始化和释放，每次都会调用
使用单例
写个单例，在构造函数中初始化网络资源
在析构函数中释放网络资源


### v3
使用RabbitMQ消息队列

使用docker拉取带管理插件的 RabbitMQ 镜像，包含 Web 可视化管理界面
```
docker pull rabbitmq:management
```

启动RabbitMQ
```
docker run -d  --hostname rabbitsrv  --name rabbit  -p 5672:5672  -p 15672:15672  -p 25672:25672  -v /data/rabbitmq:/var/lib/rabbitmq  rabbitmq:management
```



### v4

项目目录
```
proto
│
├── user.proto
│
└── file.proto


UserServer
│
├── UserServiceImpl.cc
├── UserServiceImpl.h
│
├── user.pb.cc
├── user.pb.h
│
├── user.srpc.cc
└── user.srpc.h


FileServer
│
├── FileServiceImpl.cc
├── FileServiceImpl.h
│
├── file.pb.cc
├── file.pb.h
│
├── file.srpc.cc
└── file.srpc.h


Gateway
│
├── UserRpcClient.h
├── FileRpcClient.h
│
└── HttpHandlers.cc
```


Gateway在main进程中执行
在原来的user_handler函数中创建UserClient对象，调用UserServer服务器即可
需要向UserServer服务器发送username、password
UserServer创建MySQLTask，插入用户数据，获取自增id

再将执行结果返回给Client



user.proto
```
syntax = "proto3";

package user;

message RegisterRequest
{
    string username = 1;
    string password = 2;
}

message RegisterResponse
{
    int32 code = 1;
    string msg = 2;

    int32 user_id = 3;
    string username = 4;
}

message LoginRequest
{
    string username = 1;
    string password = 2;
}

message LoginResponse
{
    int32 code = 1;
    string msg = 2;

    int32 user_id = 3;
    string username = 4;

    string token = 5;
}

message UserInfoRequest
{
    int32 user_id = 1;
}

message UserInfoResponse
{
    int32 code = 1;
    string msg = 2;

    int32 user_id = 3;
    string username = 4;
}

service UserService
{
    rpc Register(RegisterRequest)
        returns(RegisterResponse);

    rpc Login(LoginRequest)
        returns(LoginResponse);

    rpc GetUserInfo(UserInfoRequest)
        returns(UserInfoResponse);
}

```


### v5

```
docker run --name consul1 -d -p 8500:8500 -p 8301:8301 -p 8302:8302 -p 8600:8600  hashicorp/consul agent -server -bootstrap-expect 2 -ui -bind=0.0.0.0 -client=0.0.0.0
```