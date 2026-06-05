push失败问题：
`更改了配置文件`

![[push 失败.png]]
![[push失败-pull也有问题.png]]

解决方法：
```c
# 1. 配置为合并策略（如果还没有配置）
git config pull.rebase false

//更改配置文件：nano文件编辑器
//按ctrl+o保存，ctrl + x退出

# 2. 拉取远程更改
git pull origin master

# 3. Git会自动创建一个合并提交
# 如果有冲突，需要手动解决冲突
# 解决冲突后：
git add .
git commit -m "合并远程更改"

# 4. 重新推送
git push origin master
```
