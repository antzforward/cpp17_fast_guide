#include <iostream>
#include <vector>
#include "../TestMacro.h"
#include "inlinethreadlocal.hpp"
/**
 * @file 30inlineInHeadFile.cpp
 * @ingroup 01Basics
 * @brief C++17 inline 变量和 thread_local — 全局共享对象的定义方式
 * @details 演示 C++17 引入的 `inline` 变量特性：
 *          - 允许在头文件中定义全局变量，无需担心多重定义链接错误
 *          - 配合 `thread_local` 实现线程本地存储
 *          - 演示跨线程访问共享数据时各变量的作用域差异
 *          - 演示 struct 内 static constexpr 成员的 C++17 定义方式
 */

void foo()
{
	myThreadData.print("foo() begin:");
	myThreadData.gName = "thread2 name";
	myThreadData.tName = "thread2 name";
	myThreadData.lName = "thread2 name";
	myThreadData.print("foo() end:");
}
int twice(const int& i){
	return i*2;
}
// 使用示例
int main() {
	// added the two lines below
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(NULL);  
    {
		myThreadData.print("main() begin:");
		myThreadData.gName = "thraed1 name";
		myThreadData.tName = "thread1 name";
		myThreadData.lName = "thread1 name";
		myThreadData.print("main() later:");
		std::thread t(foo);
		t.join();
		myThreadData.print("main() end:");
    }
    {
		std::cout<<"测试 constexpr在struct里面的声明\n";
		std::cout<<twice(MyData::n)<<'\n';//这里也是OK的。
		std::cout<<"测试结束\n";
    }
    return 0;
}
//cl /EHsc /std:c++17 30inlineInHeadFile.cpp /link psapi.lib 