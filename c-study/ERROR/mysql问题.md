##### 使用capi连接数据库显示
`ALTER USER 'root'@'localhost' IDENTIFIED WITH mysql_native_password BY '123456';`

查看root用户认证插件
`SELECT user, plugin FROM mysql.user WHERE user='root';`
原因是mysql中root用户的认证插件是suth_socket，不关注密码，只看运行C程序的操作用户是不是root

修改认证方式
`ALTER USER 'root'@'localhost' IDENTIFIED WITH mysql_native_password BY '123456';
FLUSH PRIVILEGES;`

失败，提示
`ERROR 1819 (HY000): Your password does not satisfy the current policy requirements`

需要修改mysql数据库的密码强度等级

查看密码强度等级
`SHOW VARIABLES LIKE 'validate_password%';`

修改等级为low
`SET GLOBAL validate_password_policy = LOW;`

调整密码的最小长度
`SET GLOBAL validate_password_length = 4;`

就可以使用密码修改root的认证方式
`ALTER USER 'root'@'localhost' IDENTIFIED WITH mysql_native_password BY '123456';`

