#include <iostream>
#include <vector>
#include <utility>
#include <memory>
#include <atomic>
#include "../TestMacro.h"
/**
 * @file 52CopyElision.cpp
 * @ingroup 01Basics
 * @brief 强制删除拷贝/移动后的省略策略 — 工厂函数与引用传递的对比
 * @details 在 50CopyElisison.cpp 的基础上进一步实验：
 *          - 当显式 `= delete` 拷贝和移动构造函数后，按值传递完全不可用
 *          - 使用泛型工厂函数 `create<T>()` 的替代方案
 *          - 按引用传递（const& / &）在删除拷贝/移动后的性能表现
 *          - 结论：对于复杂对象，按引用传递仍然是最实用的方案
 */
// 50CopyElisison.cpp 里面还是有临时变量拷贝
// 这里强制省略之。这次是在std的机制下面进行保证，直接减少拷贝
// 从测试结果来看，test2性能下降非常明显。感觉删除移动操作还是变慢了。移动带来的性能下降很明显的。
//这个用例用来处理，显示删除了拷贝与移动的形式。这里我还是用50CopyElisison.cpp 的class定义
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
	//MyClass2(const MyClass2&) = default;//a=b拷贝赋值对应的
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
// 这里介绍使用工厂模式的代码。就是使用create，unique ptr的形式
// 但是书上的用例，我在不引入工厂方法的情况下就能编译通过的，所以不明白
// 翻译者为什么要说，这个是泛型工厂函数的作用。
// 所以 我修改了泛型工厂函数的用法，也就是所有的函数都编程较为复杂一点的形式
// 明确指定create的形式，来决定它的后面形式--- 另外一种危险情况的来源。
// factory 函数
template <typename T,typename... Args>
T create(Args&&... args)
{
	return T{std::forward<Args>(args)...};
}
//以为这个工厂函数为“总是”可以工作的，我copy了删掉了拷贝移动函数的class，用create的形式来替代一下
// 发现不行，还是把函数改成引用类型吧
class MyClass3
{
public:
	inline static std::string gName ="global";// 整个程序中只有一个
	inline static thread_local std::string tName = "tls"; // 每个线程有一个
	std::string lName = "local"; //每个实例有一个
	static constexpr int n = 5; // C++11/C++14: 声明
								// 自从C++17起: 定义
	int value;
	//没有拷贝/移动构造函数的定义,导致编译不过，但是需要这个设定-优化形式
	//这里使用工厂函数的形式来处理
	MyClass3(const MyClass3&) = delete;//a=b拷贝赋值对应的
	MyClass3(MyClass3&&) = delete;//std::move对应的
};
// param 用传递进入的实参初始化
// 常规的优化，是传入引用，即[const]MyClass2& param
// 但是不符合现在讨论的情况
void foo(const MyClass3& param)
{
}
MyClass3& bar(MyClass3& obj){// obj 按值传递
	obj.value += 1;
	return obj;// obj 是左值！
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
		下面是这次测试数据
		PERF [test1]:
		  Time:  1012.36 ms
		  Memory:24576 bytes (24 KB)
		  Peak:  2812 KB
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
		下面是本次测试
		PERF [test2]:
		  Time:  3050.1 ms
		  Memory:0 bytes (0 KB)
		  Peak:  2848 KB		
		*/
		PERF_START(test2)
		
		/** 测试代码 - 分配一些内存*/
		for (int i = 0; i < N; i++) {
			foo(MyClass2{});//此处要执行移动与拷贝两个操作
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
		//本次代码执行性能
		PERF [test3]:
		  Time:  922.682 ms
		  Memory:0 bytes (0 KB)
		  Peak:  2852 KB		
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
    {
		//factory 函数的测试
		int i = create<int>(42);
		std::unique_ptr<int> up = create<std::unique_ptr<int>>(new int{42});
		std::atomic<int> ai = create<std::atomic<int>>(42);
    }
    {
		// 可以发现这种复杂的左值右值改进，还是为了应对哪些没用引用形式的代码
		/**
		PERF [test4]:
		  Time:  23.6879 ms
		  Memory:0 bytes (0 KB)
		  Peak:  2668 KB		
		*/
		PERF_START(test4)
		
		/** 测试代码 - 分配一些内存*/
		for (int i = 0; i < N; i++) {
			foo(MyClass3{});//此处要执行移动与拷贝两个操作
			MyClass3& x = bar(MyClass3{});
			foo(bar(MyClass3{}));
		}
		
		PERF_END(test4) 
    }
    return 0;
}
//cl /EHsc -O2 /std:c++17 52CopyElision.cpp /link psapi.lib 