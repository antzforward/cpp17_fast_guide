#include <iostream>
#include <vector>
#include "../TestMacro.h"
#include "inlinethreadlocal.hpp"

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