将4个数字分离两两一对
1120
1120/100-->11
1120%100-->20

将数字拆成小时+分钟
790 
790 / 60 -->13小时
790 %60 -->10分钟

scanf 后边记得取地址
```
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
int main() {
	int hour, minte;
	scanf("%d %d", &hour, &minte);
	int h = hour / 100;
	int m = hour % 100;
	int total = h * 60 + m + minte;
	int re = (total / 60 )* 100 + total % 60;
	printf("%d", re);
	return 0;
}
```

进制转换
十进制  --> 十六进制
18       -->   0x12
18  /  16 -->1
18 % 16 --> 2

将3位数提取
352
千  352 / 100 =3
百  352 / 10 = 35 % 10 =5 
个  352 % 10 =2

将一个数取余 --> 去掉前面位，保留后边数
352 % 10 --> 2
352 % 100 --> 52
将一个数除 --> 去掉后面位，保留前边数
352 / 100 --> 3
352 / 10 --> 35

用循环判断一个数的位数
```
	int x;
	int n = 0;
	scanf("%d", &x);
	n++;              #数一进来就是一位数
	x /= 10;
	while (x>0)       #循环几次，位数就是n+1
	{                 #352 循环2次，位数为3
		n++;
		x /= 10;
	}
	printf("%d\n", n);
```
do-while循环
先循环再判断，若条件满足则继续循环，条件不满足则跳出循环
```
	int x;
	int n = 0;
	scanf("%d", &x);
	do
	{
		n++;
		x /= 10;
	}while (x>0);
	printf("%d\n", n);
```

猜随机数游戏
机器：循环条件是满足判断的条件
人：循环结束的条件
思考方式相反，循环结束的条件是猜的数与随机数相同
循环开始的条件是猜的数与随机数不同
将人的思维转换成机器的思维方式
```
#include<stdlib.h>
#include<time.h>

	srand(time(0));
	int number = rand()%100+1;
	int count = 0;
	int a = 0;
	printf("我已经想好了一个1-100的数");
	do {
		printf("请猜这个1-100之间的数");
		scanf("%d", &a);
		count++;
		if (a > number) {
			printf("你猜的数大了");
		}
		else if (a < number) {
			printf("你猜的数小了");
		}
	} while (a != number);
	printf("你用了%d次就猜到了答案。\n",count);
}
```
一定要猜，所以要先循环再判断条件-->使用do-while循环


求平均数
先找出所需的变量
```
	int number;
	int sum = 0;
	int count = 0;
	scanf("%d", &number);
	while (number != -1) {
		sum += number;
		count++;
		scanf("%d", &number);    #给出再一次循环的条件
	}
	printf("%f\n", 1.0 * sum / count); #精确平均数，变成浮点数
```


整数求逆
```
	int x;
	x = 700;
	int digit;
	int ret = 0;
	while (x > 0) {
		digit = x % 10;
		ret = ret * 10 + digit;
		printf("x=%d,digit=%d,ret=%d", x, digit,ret);
		x /= 10;
	}
	printf("%d", ret);
	
	-->去掉尾数0
```  

```
	int x;
	x = 700;
	int digit;
	int ret = 0;
	while (x > 0) {
		digit = x % 10;
		printf("%d",digit);
		ret = ret * 10 + digit;
		x /= 10;
	}
	
	-->不去尾数0
```

for=while、do-while
for(初始动作 ; 条件 ; 每轮动作 ){
  for中的每个表达式都可以省略
}
```
如果有固定次数，用for
如果必须执行一次用do-while
其他情况用while
```

判断一个数是否为素数
```
	int x;
	scanf("%d", &x);
	int i;
	int isPrime = 1;
	for (i = 2; i < x; i++) {
		if (x % i == 0) {
			isPrime = 0;
			break;
		}
	}
	if (isPrime == 1) {
		printf("是素数\n");
	}
	else {
		printf("不是素数\n");
	}
```

输出100以内的素数

break和continue的区别
	break是跳出当前循环
	continue是跳过当前循环，进行此循环的下一轮循环
```
	int x;
	for (x = 2; x <= 100; x++) {
		int i;
		int isPrime = 1;
		for (i = 2; i < x; i++) {
			if (x % i == 0) {
				isPrime = 0;
				break;
			}
		}
		if (isPrime == 1) {
			printf("是素数\n");
		}
	}
```

输出前50个素数
```
	int x;
	int count = 0;
	x = 2;
	while (count < 50) {
		int i;
		int isPrime = 1;
		for (i = 2; i < x; i++) {
			if (x % i == 0) {
				isPrime = 0;
				break;
			}
		}
		if (isPrime == 1) {
			count++;
			printf("%d\t", x);
			if (count % 5 == 0) {
				printf("\n");
			}
		}
		x++;
	}
```

1角、2角、5角凑x元
	tips：
	将单位统一，元--->角
	范围限定，每种硬币数量的最大值确定好
```
	int x;
	scanf("%d",&x);
	int one, two, five;
	int exit = 0;
	for (one=1; one < x * 10; one++) {
		for (two=1; two < x * 10 / 2; two++) {
			for (five=1; five < x * 10 / 5; five++) {
				if (one + 2 * two + 5 * five == x * 10) {
					printf("%d个1角%d个2角%d个5角凑%d元\n", one, two, five, x);
					exit = 1;
					break;         #接力break，跳出三层循环
				}
			}
			if (exit == 1)break;
		}
		if (exit == 1)break;
	}
```

前n项求和
```
	int i;
	int n;
	double sum = 0.0;       #double类型初始化为0.0
	scanf("%d", &n);
	for (i = 1; i <= n; i++) {
		sum += 1.0 / i;           #单位统一都是浮点数
	}
	printf("前%d项和为%f", n, sum);
```
正负号交替
1-1/2+1/3-1/4...
```
	int i;
	int n;
	double sum = 0.0;
	double sigh=1;
	scanf("%d", &n);
	for (i = 1; i <= n; i++) {
		sum += sigh / i;
		sigh = -sigh;
	}
	printf("前%d项和为%f", n, sum);
```

pow(10,2)   ==>10的2次方
正序输出整数
```
	int x;
	scanf("%d", &x);
	int mask = 1;
	int t = x;            #求mask的位数，只需在1后乘循环次数-1个0
	while (t > 9) {       #循环次数-1是在t>0的情况下，t>9是循环次数
		t /= 10;
		mask *= 10;
	}
	printf("mask=%d,t=%d\n", mask, t);
	while (mask > 0) {        #若为x>0则x按照以下计算会去掉0，要按mask的衰减来算
		int d = x / mask;
		printf("%d", d);
		if (mask > 9) {       #当mask<9时，x只剩个位数，个位数后不加空格
			printf(" ");
		}
		x %= mask;
		mask /= 10;
	}
	printf("\n");
```

辗转相除法计算最大公约数
1、如果b=0，计算结束，a就是最大公约数
2、否则，计算a除以b的余数，让a=b，b=余数
3、回到第一步
```
int a,b;
int t;
scanf("%d %d",&a,&b);
a=12;b=18;
while(b!=0){
	t=a%b;
	a=b;
	b=t;
}
printf("gcd=%d",a);	
```

求符合条件的整数集
从a开始（a不超过6）的连续4个数字，组成的3位数，每行输出6个整数，用空格分隔，最后一个数字后无空格
```
int main() {
	int a;
	scanf("%d", &a);
	int count = 0;
	for (int i = a; i <= a + 3; i++) {
		for (int j = a; j <= a + 3; j++) {
			for (int k = a; k <= a + 3; k++) {
				if (i != j && i != k && j != k) {
					printf("%d", i * 100 + j * 10 + k);
					count++;
					if (count % 6 == 0) {
						printf("\n");
					}
					/*if (count == 6) {
						printf("\n");
						count = 0;
					}*/
					else {
						printf(" ");
					}
				}
			}
		}
	}
	return 0;
}
```

求水仙花数
输入n（n>=3） 输出所有n位数的水仙花数  153=1^3 + 5^3 +3^3
步骤：
	变量  n、i满足i< n
	分解每位数
```
	int n;
	scanf("%d", &n);
	int i = 1;
	int first = 1;
	while (i < n) {
		first *= 10;
		i++;
	}
	printf("%d\n", first);
	i = first;
	while (i < first * 10) {
		int t = i;
		int sum = 0;
		do {
			int d = t % 10;
			t /= 10;
			int p = d;
			int j = 1;
			while (j < n) {
				p *= d;
				j++;
			}
			sum += p;
		} while (t > 0);
		if (sum == i) {
			printf("%d\n", i);
		}
		i++;
	}
```

九九乘法表
给定任意1位正整数n，下三角输出九九乘法表，等号右边数字占4位，左对齐
```
int n;
	scanf("%d", &n);
	int i, j;
	i = 1;
	while (i <= n) {
		j = 1;
		while (j <= i) {
			printf("%d*%d=%d", j, i, i * j);
			if (i * j < 10) {
				printf("   ");
			}
			else {
				printf("  ");
			}
			j++;
		}
		printf("\n");
		i++;
	}
```

统计素数求和
判断m和n之间有多少个素数并求出素数之和
m,n,count,sum
```
	int m, n;
	scanf("%d %d", &m, &n);
	int cnt = 0;
	int sum = 0;
	int isprime = 1;
	if(m==1) m=2;
	for (int i = m; i < n; i++) {
		for (int k = 2; k < i; k++) {
			if (i % k == 0) {
				isprime = 0;
				break;
			}
		}
		if (isprime == 1) {
			printf("%d", i);
			cnt++;
			sum += i;
		}
	}
```

猜数字游戏
系统随机产生一个100以内的正整数，用户进行猜测，将猜测的数字与系统数字比较，大了提示（Too big）小了（Too smal
）相等表示猜到了，结束程序。要求统计猜的次数，如果一次猜到输出（Bingo），3次以内猜到输出（Lucky you），超过3次但是在n次之前（包括n次）猜到则输出（Good Guess），超过n次都没有猜到输出（Game Over）并结束程序。如果在到达n次之前，用户输入了一个负数，也输出Game Over并结束程序。
```
	int m, n;
	scanf("%d %d", &m, &n);
	int cnt = 0;
	int finished = 0;
	int x;
	do {
		scanf("%d", &x);
		cnt++;
		if (x < 0) {
			printf("Game Over\n");
		}
		else if (x > m) {
			printf("Too Big\n");
		}
		else if (x < m) {
			printf("Too Small\n");
		}
		else {
			if (cnt == 1) {
				printf("Bingo\n");
			}
			else if (cnt <= 3) {
				printf("Lucky You\n");
			}
			else {
				printf("Good Guess\n");
			}
			finished = 1;  //终止do-while循环
		}
		if (cnt == n) {
			if (!finished) {
				printf("Game Over\n");
				finished = 1;
			}
		}
	} while (!finished);  //finished=0时
	return 0;
```





猴子吃桃问题:
猴于第一天摘下若干个桃子，当即吃了一半，还不过瘾，又多吃了一个，第二天早上又将第一天剩下的桃子吃掉一半，又多吃一个，以后每天早上都吃了前一天剩下的一半零一个，到第10天早上想再吃时，发现只剩下一个桃子了。
编写程序求猴子第一天共摘了多少个桃子。
倒推法：
先算第9天的，第8天的找规律即可
10天：1
9天  ：2 x 1+1
昨天/2 -1 = 今天
昨天/2 = 今天+1
昨天=今天x2+2
递推公式：2n X 2

循环条件：1-9天未知，10天是1个
for(int i=1;i<=9;i++);






















