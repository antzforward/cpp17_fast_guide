## 为什么
还是因为要转向UE 引擎了，虽然主要的工具可能是蓝图，但是，不能只有一种方式吧。复习一下c++编程。
现在主要的c++应该是c++17，但是如果设计到Task，可能还需要更新的版本，但是c++17应该已经覆盖了很多问题的。

## 怎么做
多做练习，多进行性能比较，写一堆的单一的小代码，通过不同的性能比较，建立直接印象，在做性能方向的决策的时候，就有点用。

## 重点
还是单一的cpp，单独通过cl来编译，运行的方式，不用IDE来处理，主要的文本工具就是现在用的Notepad3
没有代码自动补全，要自己手动处理。
另外，尽量在代码注释中说明自己的代码的内容，处理结果，如果是多方案的话，给出推荐方案。
比如下面的代码
```c++
//const的，会在constexpr中表现出来
#include <iostream>
#include <random>

constexpr int fibonacci( const int n ) {
	return n==1 || n==2 ?1:fibonacci(n-1) + fibonacci(n-2);
}

int main(){
	int sumIdx = 0,sum=0;
	for(int i=5; i<10;i++)//这是核心，输入参数是常量，从而上面的过程也是
	{
		//std::cout<<"fibonacci("<<i<<"): "<<fibonacci(i)<<std::endl;
		sumIdx += i;
		sum += fibonacci(i);
	}
	
	std::random_device rd; // 用来获得种子值
    std::mt19937 gen(rd()); // Mersenne Twister引擎
    // 定义一个范围为1-6的均匀分布
    std::uniform_int_distribution<> dis(5, 10);

	int sumIdx2 = 0,sum2=0;
    // 生成随机数
    for (int i = 5; i < 10; ++i) {
		auto p = dis(gen);
		sumIdx2 += p;
		sum2 += fibonacci(p);
        //std::cout<<"fibonacci("<<p<<"): "<<fibonacci(p)<<std::endl;
    }
    std::cout<<sumIdx2-sumIdx<<sum2-sum<<std::endl;
    return 0;
}
//编译未优化的asm文件，并不链接的方式 cl /EHsc /FA /c 02constexpr.cpp
```
之前把结论转入到md文件了，这次先在代码中写，然后再提纯一次吧。
