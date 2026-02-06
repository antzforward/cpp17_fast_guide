#pragma once //等价于宏包裹形式
#include <iostream>
#include <string>
#include <vector>
#include <thread>

////出于可移植性和易于整合的目的，在头文件中提供完整的类和库的定义是很重要的。然而，在C++17之前，
////只有当这个库既不提供也不需要全局对象的时候才可以这样做。
////只要一个编译单元内没有重复的定义即可。此例中的定义即使被多个编译单元使用，也会指向同一个对象。
////根据一次定义原则(ODR)，一个变量或实体的定义只能出现在一个编译单元内——除非该变量或实体被定义为inline的。
struct MyData{
	inline static std::string gName ="global";// 整个程序中只有一个
	inline static thread_local std::string tName = "tls"; // 每个线程有一个
	std::string lName = "local"; //每个实例有一个
	static constexpr int n = 5; // C++11/C++14: 声明
								// 自从C++17起: 定义
	
	void print(const std::string& msg) const{
		std::cout<<msg<<'\n';
		std::cout << "- gName: " << gName << '\n';
		std::cout << "- tName: " << tName << '\n';
		std::cout << "- lName: " << lName << '\n';
		std::cout << MyData::n<<'\n';
	}
};
inline thread_local MyData myThreadData;// 每个线程一个对象
inline MyData myMyDataObj; // 即使被多个CPP文件包含也OK
inline thread_local std::vector<std::string> cache; // 每个线程都有一份cache

//// 以前的做法，是在h文件内声明变量，或者getter，然后在hpp文件里面定义变量
//// 可以说c++17之后，这种方式提高了代码的可读性。
