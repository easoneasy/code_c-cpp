
注册：用户输入用户名+密码
服务器会随机生成盐值
将用户的密码和随机生成的盐值哈希
得到密码之后放入数据库表的密码表项

登录：用户输入用户名+密码
服务器先查数据库，查询密码+盐值
服务器计算用户输入的密码+从数据库中查询到的盐值
哈希之后对比密码是否匹配

Q：数据库被拖库了，攻击者已经拿到了 password 和 salt，那盐还有什么意义？
A：盐不是为了保密，而是为了防止彩虹表攻击和相同密码产生相同哈希值。

彩虹表：预先计算好的  密码 <---> 哈希值  的对照表
加盐可以防彩虹表



步骤：

```
① HTTP服务器

② 文件下载

③ MySQL连接

④ 注册

⑤ 登录

⑥ Token

⑦ Token超时

⑧ 文件鉴权
```

###### 第一阶段：先实现最简单的静态资源服务器

不要登录。

不要数据库。

不要Token。

只做：

```
GET /files/test.txt↓返回 test.txt
```


路由：

```
if(path.find("/files/")==0){    handle_file();}
```


handle_file：

```
open()pread()append_output_body()
```

先把这个跑通。

浏览器：

```
http://127.0.0.1:8848/files/test.txt
```

能下载文件。


这一步你应该自己能写。

因为本质上就是 Workflow 文件IO。


###### 第二阶段：实现注册

此时不要登录。

不要Token。

只实现：

```
POST /register
```

请求：

```
username=tom&password=123456
```


你要学会：

```
req->get_parsed_body()
```

解析：

```
usernamepassword
```


然后：

```
INSERT INTO tbl_user
```

这一步核心知识：

```
Workflow MySQL任务
```


整个流程：

```
HTTP
 ↓
解析表单
 ↓
拼SQL
 ↓
WFMySQLTask
 ↓
插入数据库
```
###### 第三阶段：实现登录

这个阶段开始难了。

请求：

```
POST /login
```

根据用户名：

```
select password,salt
```

取出：

```
数据库密码数据库盐值
```

重新计算：

```
hash(    用户输入密码    +    数据库盐值)
```

比较。

成功：

```
200
```

失败：

```
401
```

到这里其实已经完成：

```
登录认证
```

###### 第四阶段：实现Token

这个阶段很多同学最容易懵。

其实先不要JWT。

不要签名。

不要加密。

先做一个最简单版。

例如：

```
unordered_map<    string,    string> token_map;
```

登录成功：

```
string token =    random_string();
```

保存：

```
token_map[token]=username;
```

返回：

```
{    "token":"abc123"}
```

下载文件：

```
Authorization:Bearer abc123
```

验证：

```
if(token_map.find(token)   == token_map.end()){    401}
```

先把流程打通。

###### 第五阶段：Token超时

这时候再加：

```
struct TokenInfo{    string username;    time_t expire;};
```

变成：

```
unordered_map<    string,    TokenInfo>;
```

登录：

```
expire=time(nullptr)+1800;
```

验证：

```
if(now>expire){    401}
```








##### 流程



#### 注册
1、解析uri中的uername 、 password
```
POST /register
      ↓
解析用户名密码
      ↓
生成salt
      ↓
生成hash
      ↓
创建MySQLTask
      ↓
保存Context
      ↓
push到Series
      ↓
（异步执行）
      ↓
register_callback
      ↓
返回结果
```

```
耗时操作
    ↓
Task

多个步骤
    ↓
Series

跨步骤数据
    ↓
Context

任务结果处理
    ↓
Callback
```