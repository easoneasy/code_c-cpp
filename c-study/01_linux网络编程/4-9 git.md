
```c
//工作区的变化
git status

//将工作区的变化放在暂存区
git add 文件名

//将暂存区的内容放在本地仓库中
git commit -m  "备注内容"

//将本地仓库的内容放入远程仓库
git push

//查看提交日志
git log
git log --oneline 
git log --oneline --all
```

![[git图解.png]]
![[Pasted image 20260412155754.png]]

`git add .`  把工作区所有的变化推到暂存区

`git log --oneline`  只显示所在分支的提交记录

`git log --oneline --all`  显示所有分支的全部提交记录


分支branch
开发过程中有一个主干，主干代码是可以直接运行，有新的开发需求、解决代码问题等都会从主干上复制代码到分支上进行开发，完成之后再将分支上的代码合并到主干上。
```c
//查看分支
git branch

//查看所有分支
git branch -a

//新建 dev分支
git branch dev
//在哪个分支里，哪个分支前面会有*

//老版本切换到dev分支
git checkout dev

//新git版本中，版本切换命令
git switch dev

//合并分支 dev,将dev分支合并到当前分支
//执行这行代码的时候要切换到要合并到的目标分支
//将A分支合并到B分支，要先切换到B分支，在B分支执行merge命令
git merge dev
```

切换分区的时候，要保证工作区与工作区是干净的

在合并之前dev分支和master的分支内容不同

![[git - branch分支.png]]

切换到master，将dev分支中的内容合并到master
![[git_branch_merge.png]]

关系分支
将dev的文件合并到merge，直接合并没有问题
![[git_branch_graph.png]]


master和dev都修改了同一个文件（距离远的不同行）
合并到master不会冲突

master和dev都修改了同一个文件（距离近的不同行）
合并到master会冲突，
需要手动更改（也可以不更改），再add-->commit


远程仓库的内容改变了，本地此时也改变了内容。
1、可以使用`git fetch origin 分支名`将远程仓库的内容拉到本地对于远程的缓存上
（origin/master本地缓存，相当于一个分支，与master不同）

2、之后使用`git merge origin/分支名`将本地缓存origin/master中的内容（远程仓库更新的内容）和本地仓库更新的内容合并

3、最后再使用`git push origin 分支名`推到远程仓库中


origin/master本地仓库缓存

```c
git fetch origin 分支名


git merge origin/分支名


git push origin 分支名
```



回退文件

如果在工作区修改错了文件可以直接删除
```c
rm -f 文件名
```

文件已经add到暂存区，这个命令可以回退到工作区
```c
git restore --staged 文件名
```

如果误删一个文件且已经git add .
```c
git status
//从暂存区取消删除
git restore --staged del_txt.c
//丢弃工作区的改动
git restore del_txt.c
```

如果一个文件已经git commit 到本地仓库
```c
git reset --hard 要恢复到哪个版本号
```

![[git_reset.png]]

![[git_reset1.png]]