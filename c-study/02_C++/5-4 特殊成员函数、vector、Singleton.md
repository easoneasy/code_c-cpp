##### 静态成员函数
static修饰成员函数
	不依赖对象，依赖类
	一般可以使用类名作用域来访问，可以用对象.访问，但看不出来是静态
	静态数据成员函数无法直接访问非静态的成员（数据成员/函数）
		可以间接访问，需要创建对象
	没有this指针
应用场景：
	可以作为工具类的方法/函数
		工具类：任何人都可用的类，大部分都要用到的类，可以减少代码冗余
			public权限，任何人都可以使用，例如：DBUtils，数据库工具类

静态成员函数  vs  非静态成员函数
静态成员函数：
	访问静态成员：OK
	**访问非静态成员：NO**
非静态成员函数：
	访问静态成员：OK
	访问非静态成员：OK

__*静态成员函数为什么不能直接访问非静态成员*__
因为没有this指针，不知道访问哪个对象的成员

##### const成员函数
在成员函数的形参列表之后，加const
只让访问，不让修改
语法：
	返回值类型  函数名(形参列表)  const
特点：
	const 和成员函数结合，具有只读属性，不能修改对象状态
	即数据成员的取值不能修改

const 成员函数的this指针是双重const限定的指针，既不能改指向，也不能改内容
`const 类名  * const`

可以构成成员函数的重载
原因是函数中隐藏的this指针类型不同

类中有const版本和非const版本的成员函数
普通对象调用非const的函数，没有非const函数也能调用const函数
**const对象只能调用const的函数**

***this指针在const成员函数和非const成员函数中的类型有什么不同***
const成员函数中，this指针的类型是`const 类名 * const`
非const成员函数中，this指针的类型是`类名 * const`

##### 对象数组
构建对象数组形式：
	通过左值对象构建数组
	通过右值对象构建数组
	通过初始化列表简写构建数组
对象作为数组元素时，会调用拷贝构造函数
```c++
Point pt1{ 1, 1 };
Point pt2{ 2, 2 };
Point pt3{ 3, 3 };
// 对象作为数组元素时 会调用copy
Point pts[3] = { pt1, pt2, pt3 };
/* pts[0].m_x = 10; */  //只会赋值给临时对象
pts[0].print();   // 1,1
```

for-each遍历(增强for循环，迭代循环)
```c++
for(Point pt: pts){
	pt.print();
}
//副本操作，会执行拷贝构造函数
//对原来数组中的数据没有任何影响
```

使用关键字auto进行类型的自动推导
```c++
//编译器自动推断pt是什么类型的
for(auto pt : pts)
{
	pt.print();
}
//加上引用，就不会复制对象，不会调用拷贝构造函数
for(auto & pt : pts)
{
	pt.print();
}
```


栈对象和堆对象的调用方式



对象本身是operator delete释放
`delete p`  / `delete [] p` 释放的是数据成员的空间


创建堆上的对象需要什么条件
	构造函数、operator new、operator delete需要有public权限
创建栈上的对象需要什么条件
	构造函数、析构函数需要有public权限

只允许创建堆对象
	析构函数设置私有权限
只允许创建栈对象
	operator new 、operator delete 设置为private权限
##### ==单例设计模式==
###### 单例对象创建在栈上：
	private修饰构造函数（构造函数私有）
	提供一个public static的函数返回这个唯一的对象
	在函数中保证该对象唯一
		通常可以把拷贝构造、赋值运算符删掉/设置为私有
单例对象：创建在静态区

- **🔒 构造函数私有** $\rightarrow$ 外部无法 `new` 或直接声明对象 $\rightarrow$ 导致无法通过 `对象.函数` 访问。
- **🛠️ static 成员函数** $\rightarrow$ 脱离对象限制，直接通过 `类名::函数` 访问 $\rightarrow$ 解决了“进不去门”的问题。
- **🏢 静态局部变量** $\rightarrow$ 存放在静态存储区，生命周期贯穿程序始终，且只会被初始化一次。
- **🔗 返回引用** $\rightarrow$ 确保外部拿到的就是那个唯一的“真身”，而不是一份临时的拷贝。
```c++
class Singleton
{
public:
    static Singleton & getInstance()
    {
        static Singleton instance;
        // 返回一个唯一的对象
        return instance;
    }

    // 删除复制控制的函数
    Singleton(const Singleton &) = delete;
    Singleton& operator=(const Singleton &) = delete;
private:
    Singleton(){}
};

void test1()
{
    /* Singleton s1; */
    /* Singleton s2; */
    /* Singleton s3; */
    // 对象.getInstance();
    cout << &Singleton::getInstance() << endl;
    cout << &Singleton::getInstance() << endl;
    cout << &Singleton::getInstance() << endl;
}
```

把拷贝构造函数、赋值运算符函数删掉
```c++
Singleton (const Singleton &) = delete;
Singleton & operator=(const Sigleton &) = delete;
```

###### 单例对象创建在堆上
	构造函数设置为私有
	提供一个外部访问入口的public static函数返回这个唯一的对象
	函数中保证整个对象是唯一的
	提供一个自身类型的指针，给指针赋值，通过判断指针是否为NULL，看是否赋值

(线程不安全的单例模式)
```c++
class Singleton
{
public:
	static Singleton *getInstance()
	{
		//判断指针是否为空
		if(!m_pInstance)
		{
			m_pInstance = new Singleton{};
		}
		return m_pInstance;
	}
	//释放资源的操作  //删除对象
	static void destroyInstance()
	{
		if(m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}	
	//删除复制控制相关的函数
	Singleton(const Singleton &) = delete;
	Singleton & operator = (const Singleton &) = delete;
private:
	Singleton(){}
	//增加一个自身类型的指针
	static Singleton *m_pInstance;
};
//类外初始化
Singleton *Singleton::m_pInstance = nullptr;
```

静态区和堆区实现的优缺点：
静态区实现：简单，线程安全，对象生命周期与程序相同，占用静态区内存
堆区实现：可以手动控制对象销毁，适合大对象，处理线程安全和手动释放

##### C++类型的字符串

C风格字符串：字符数组，以\0结尾，需要手动管理内存，使用库函数（strcpy、strcat）操作，容易出错（缓冲区溢出、忘记结尾空字符等）
C++ std::string：封装了内存管理，避免C风格字符串的常见错误，提高代码的可读性和安全性

迭代器   : 理解为广义的指针，可以像指针一样使用，但不是指针
```c++
string str{"abcdef"};
cout << str.at(0) << endl;
cout << str[1] << endl;
//使用迭代器
auto it = str.begin();
it += 2;
cout << *it << endl;
//容量
str.size()
str.empty()
//转换成C风格的字符串
str.c_str()
string str{"abcd"};
//获取首迭代器
//auto itBegin = str.begin();
string :: iterator itBegin = str.begin();

//itBegin指向第一个字符的位置
//itEnd指向字符串最后元素的下一个位置，需要--才能使用

string(InputIt first ,InputIt end)
//左闭右开

//遍历string,增强for循环
//auto会自动匹配对象
for(auto & element : str)
{
	cout << element ;
}
cout << endl;
//迭代器遍历
auto itBegin = str.begin();
for(;itBegin != str.end();++itBegin)
{
	cout << *itBegin << endl;
}
```



##### Vector容器
顺序的（底层是数组）的数据容器，可以存储任意类型的数据
可以是内置类型也可以是自定义类型

vector对象由3个指针构成，
M_start 指向当前容器中第一个元素存放的位置
M_finish指向当前容器中最后一个元素存放的下一个位置
	size() : finish - start
M_end_of_storage指向当前容器能够存放元素的最后一个空间的下一个位置
	capacity() :  end_of_storage  -  start 
特点：
	自动扩容
==扩容原理：==
	1、当size()函数的结果与capacity()函数结果相同，容器存满
	2、再往容器中存储元素就会开辟一片原空间2倍大小的空间(gcc)
	3、将容器的元素全部复制到新空间，在最后一个元素之后添加新元素
	4、回收原容器空间

常用操作：
```c++
//判空
empty();
//当前容器中元素个数
size();
//该容器最多能存放的元素个数
capacity();
//将元素添加到容器末尾
push_back();
//删除容器中最后一个元素
pop_back();
//清除容器中所有元素，不回收空间
clear();
//释放容器中多余的空间
shrink_to_fit();
//申请空间，不存放元素，预估容器需要多大的空间，直接申请
reserve();
```



##### I/O
流的四种状态
good() ：没错，流可以正常使用
bad()：物理因素可能会导致，比如磁盘坏了
fail()：输入的数据和接收的类型不匹配
eof()：流中的数据不可用 到末尾了  两次`ctrl + D`

clear() ：恢复流的状态

通用输入输出流 istream、ostream
cin：标准输入流，默认的输入设备：键盘
	本质：istream类型的全局对象
	cin对象完成一次输入后，返回值为自身对象，可以连续链式输入
	Point pt、istream cin
cout：标准输出流，默认的输出设备：终端控制台
	本质：ostream类型的全局对象
	缓冲区的大小为1024
	ostream cout


全缓冲区：程序结束时才会刷新缓冲区
行缓冲区：碰到换行符才会刷新
非缓冲区：不带缓冲区，cerr


读取文件数据
1、创建文件输入流的对象
2、利用输入运算符/函数读取数据
3、close释放资源


写数据到文件的步骤
1、创建输出流对象
2、利用write函数或输出运算符写数据
3、close

```c++
//有参构造
//如果目标文件不存在，会帮我们创建
//如果目标文件存在，默认的打开模式是out，会清空文件内容
ofstream ofs{"data.txt"};
ofs << "abc";   //输出运算符写数据
ofs.close();

//write函数写数据
string msg = "hello world";
ofs.write(msg.c_str(),msg.size());

```

##### 字符串流
数据类型之间进行转换
istringstream：string ---> 其他类型的数据
ostringstream：其他类型的数据 ---> string

基于string创建一个字符串输入流

```c++
string str = "123 456";
istringstream iss {str};
//数据转换，利用输入运算符做转换
// string ----> int
int num1;int num2;
iss >> num1 >> num2;
cout << num1 << " " << num2 << endl;

//string  --> double 
string str = "123 3.14";
int num1;double num2;
iss >> num1 >> num2;
cout << num1 << " " << num2 << endl;
```

通过istringstream解析数据
```c++
//创建流对象
ifstream ifs{"db.txt"};
//循环按行读取
string line;
while(std::getline(ifs,line))
{
	//利用istringstream做处理
	//把字符串数据交给istringstream
	istringstream iss {line};
	string key;  //存属性名
	string value; //存属性值
	iss >> key;
	iss >> value;
	cout << "key" << key << "=" << "value" << value << endl;
}
```

ostringstream 其他类型的数据 ---> string
```c++
//创建字符串输出流对象
ostringstream oss;
int num1= 123;
double num2 = 3.14;
//把需要转换的数据交给ofs,利用输出运算符
oss << num1 << " " << num2;
//通过str函数得到字符串数据
string s = oss.str();
cout << "s : "  << s << endl; 
```


##### Log4cpp组件
![[Log4cpp流程图.png]]

g++  文件名  -llog4cpp


创建输出器对象
给输出器绑定格式化器对象
获取根记录器
设置优先级
给记录器添加输出器
产生日志信息
释放资源

```c++
OstreamAppender *appender1 = new OstreamAppender("console")

PatternLayout 
```