#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include "../TestMacro.h"
/**
 * @file 50CopyElisison.cpp
 * @ingroup 01Basics
 * @brief C++17 拷贝省略（Copy Elision）— 强制省略临时对象的拷贝
 * @details 演示 C++17 引入的强制拷贝省略规则：
 *          - 当以值传递或返回临时对象时，编译器**必须**省略拷贝（guaranteed elision）
 *          - 区分 RVO（Return Value Optimization）和 NRVO（Named RVO）
 *          - 对比有/无拷贝/移动构造函数时的性能差异
 *          - 对比按值传递 vs 按引用传递的性能
 *          - 需开启 O2 优化才能观察到显著的性能差异
 *///// 强制省略拷贝或传递未实质化的对象，注意这里打开O2优化非常重要
/**
从技术上讲，C++17引入了一个新的规则：
	当以值传递或返回一个临时对象的时候，必须省略对该临时对象的拷贝
从效果上讲，我们实际上是传递了一个未实质化的对象 unmaterialized object
两种返回优化，一种叫
RVO：Return Value optimization：就是贴近参数列表或者return语句的临时未具名的函数对象的拷贝操作
NRVO：named return value optimization：就是一般对象的传递过程，这个优化就普通形式了。
使用具名变量时，这个变量就是左值，需要拷贝和移动支持
使用未具名变量时，这个变量就是右值，从这个角度来看，延迟到需要创建的时候再由编译器决定适时创建的。
*/
/**为了测试这个过程，Data的内容就设置的比较复杂一点
其实，cpp中的class 和 struct差不多，这里就用class来测试
*/
class MyClass
{
public:
	inline static std::string gName ="global";// 整个程序中只有一个
	inline static thread_local std::string tName = "tls"; // 每个线程有一个
	std::string lName = "local"; //每个实例有一个
	static constexpr int n = 5; // C++11/C++14: 声明
								// 自从C++17起: 定义
	int value;
};
void foo(MyClass param)//param 用传递进入的实参初始化
{
}
MyClass bar(){
	return MyClass{};//返回临时对象
}
class MyClass2
{
public:
	inline static std::string gName ="global";// 整个程序中只有一个
	inline static thread_local std::string tName = "tls"; // 每个线程有一个
	std::string lName = "local"; //每个实例有一个
	static constexpr int n = 5; // C++11/C++14: 声明
								// 自从C++17起: 定义
	int value;
	//没有拷贝/移动构造函数的定义,导致编译不过，但是需要这个设定-优化形式
	// 虽然不用，但是不能申明删除
	//MyClass2(const MyClass2&) = delete;//a=b拷贝赋值对应的
	//MyClass2(MyClass2&&) = delete;//std::move对应的
	//补充
	MyClass2()=default;//默认构造函数
};
// param 用传递进入的实参初始化
// 常规的优化，是传入引用，即[const]MyClass2& param
// 但是不符合现在讨论的情况
void foo(MyClass2 param)
{
}
MyClass2 bar(MyClass2 obj){//// 传递临时变量时会省略拷贝,要打开O2
	obj.value += 1;
	//// 仍然需要拷贝/移动支持
	return obj;//返回临时对象
}
/// 主要增加函数形式，传统形式。
//增加传入形式是引用的情况，用来测试性能，看传统做法是否依然有效
void foo1(const MyClass2& param)
{
}
MyClass2& bar1(MyClass2& obj){//传入引用，传出同样是引用
	obj.value += 1;
	return obj;//返回传入参数
}

int main() {
	// added the two lines below
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(NULL);  
	const int N=100'000'000;
	{
		//打开O2的情况，不开优化速度很慢
		/**
		PERF [test1]:
		  Time:  1683.02 ms
		  Memory:4096 bytes (4 KB)
		  Peak:  2612 KB
		*/
		PERF_START(test1)
		
		/** 测试代码 - 分配一些内存*/
		for (int i = 0; i < N; i++) {
			foo(MyClass{});
			MyClass x = bar();
			foo(bar());
		}
		
		PERF_END(test1) 
    }  
    {
		//打开O2的情况，不开优化速度很慢
		/**
		PERF [test2]:
		  Time:  1692.17 ms
		  Memory:0 bytes (0 KB)
		  Peak:  2648 KB
		*/
		PERF_START(test2)
		
		/** 测试代码 - 分配一些内存*/
		for (int i = 0; i < N; i++) {
			foo(MyClass2{});//尝试引用已删除的函数，此处要执行移动与拷贝两个操作
			MyClass2 x = bar(MyClass2{});
			foo(bar(MyClass2{}));
		}
		
		PERF_END(test2) 
    } 
    {
		//打开O2的情况，不开优化速度很慢
		/**
		PERF [test3]:
		  Time:  784.114 ms
		  Memory:0 bytes (0 KB)
		  Peak:  2652 KB
		*/
		//还是考虑传入引用之类的形式吧，毕竟是常用的方式
		PERF_START(test3)
		
		/** 测试代码 - 分配一些内存*/
		for (int i = 0; i < N; i++) {
			foo1(MyClass2{});
			MyClass2 x = bar1(MyClass2{});
			foo1(bar1(MyClass2{}));
		}
		
		PERF_END(test3) 
    }
    return 0;
}
//cl /EHsc /O2 /std:c++17 50CopyElisison.cpp /link psapi.lib 