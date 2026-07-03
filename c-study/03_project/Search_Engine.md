
### v1 离线部分
##### 根据语料库和停用词 生成 词典库和索引库

先创建一个`DirectoryScanner`类，
用来找到语料库和停用词的路径信息

使用`KeywordProcessor`类实现核心功能

##### KeywordProcessor

```
读取语料库
    ↓
   分词
    ↓
过滤停用词
    ↓
 统计词频
    ↓
生成词典库(dict)
```

```
根据语料库和停用词 生成 词典库和索引库

第一阶段
语料库 -> 词典库

第二阶段
词典库 -> 索引库
```

```
dict.dat
0 -> 计算机
1 -> 计算
2 -> 编程
3 -> 科学


index.dat
计 -> {0,1}
算 -> {0,1}
机 -> {0}
编 -> {2}
程 -> {2}
科 -> {3}
学 -> {3}
```

##### 将语料库网页生成网页库、网页偏移库、倒排索引库

```
xml语料
 ↓
解析xml
 ↓
得到Document对象
 ↓
去重
 ↓
生成网页库
 ↓
生成偏移库
 ↓
提取关键词
 ↓
TF-IDF
 ↓
倒排索引
```

第一步：
使用tinyxml2解析xml
放入`vector<Document> documents_`
```
tinyxml2
读取xml
↓
遍历item
↓
构造Document
↓
存入documents_
```


第二步：
去重
使用simhash比较海明距离distance <= 3，认为重复，只需保留一个，删除其它

第三步:
生成网页库，把去重之后的文档写入

第四步：
生成偏移库

第五步：
提取关键词
	统计TF：统计每篇文档的词频
	统计DF：一个词出现于多少篇文档中
	
第六步：
计算TF-IDF
	`weight = tf * log(N / (df + 1));`
	归一化：`w = w / norm;`
	
第七步：
构建倒排索引
	`unordered_map<string,vector<pair<int,double>>> invertedIndex_;`



### v2 在线部分

##### 1、关键词推荐

```
keyword
    ↓
拆分汉字
    ↓
查索引库
    ↓
得到候选词id集合
    ↓
遍历候选词
    ↓
计算编辑距离
    ↓
构造Candidate
    ↓
放入priority_queue
    ↓
取Top5
    ↓
返回
```
1、查询关键词
输入关键字
先从索引库中查找关键词对应的索引
再根据索引-->词典的下标，找到和关键词相关的词语
返回包含关键字的词语
2、计算编辑距离


##### 2、根据关键词搜索网页
```
	用户输入关键词
        ↓
	   分词
        ↓
	倒排索引查询   
        ↓
	得到候选网页集合   --> 求交集
        ↓
	计算网页相关度
        ↓
	   排序
        ↓
    --> 查offset库，读出网页，解析网页，生成摘要，返回结果
```

```
关键词
 ↓
召回候选docid
 ↓
计算网页相关度
 ↓
优先队列排序
 ↓
取TopK
 ↓
读取网页内容
 ↓
返回
```


```
vector<int> getCandidateDocIds(const vector<string> &words)

	第一个词
      ↓
	docid集合
      ↓
	result

	第二个词
      ↓
	docid集合
      ↓
	与result求交集

	第三个词
      ↓
	继续求交集
```


