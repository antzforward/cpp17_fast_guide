#include <iostream>
#include <vector>
#include <map>
#include <tuple>
#include <random>
#include <optional> //返回值，不抛出异常的形式
#include <memory> //unique_ptr 等
#include "../TestMacro.h"
/**
 * @file 20PythonWalrusOpLike.cpp
 * @ingroup 01Basics
 * @brief C++17 带初始化的 if 语句 — 类似 Python 海象运算符的用法
 * @details 对应 Python 3.8 的 `:=` 海象运算符（walrus operator），
 *          C++17 提供了 `if(init; condition)` 语法，将变量的声明和条件判断合二为一，
 *          变量作用域限制在 if-else 块内，避免外部泄露。
 *
 *          覆盖场景：
 *          - 传统写法 vs 现代写法的对比
 *          - 智能指针（unique_ptr）的生命周期管理
 *          - 容器查找（map.find）
 *          - 多变量初始化（tuple + 结构化绑定）
 *          - std::optional 的值检查
 *          - 动态类型转换（dynamic_cast）
 */
// Python := Walrus Operator（海象运算符，因为它看起来就是一个海象旋转了90°）
// 当需要同一个表达式中同时使用和测试一个值的时候，它有助于避免冗余代码--尤其是在循环或者条件语句中。
// 在c++中，这种用法叫做带初始化的if和switch语句，c++的for语句自然就有这个初始化过程，如果while不好写，用for语句吧
// 注意变量的生命周期，c++ 和 Python都是在对应语句范围内，c# 基本是个语法糖
// 这一章，主要讲带init的if 语句的情况，书上的例子缺乏单文件运行性，所以自己找例子了。
// if语句的格式是 if(initilaization_expr;conditional)
int main() {
	// added the two lines below
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(NULL);  
    {
		auto traditional_approach = []()->void{
			std::vector<int> data = {1, 2, 3, 4, 5};
			size_t n = data.size();  // 在外部声明
			
			if (n > 10) {
				std::cout << "Large vector: " << n << std::endl;
			} else {
				std::cout << "Small vector: " << n << std::endl;
			}
			std::cout<<"old approach:" << n<<std::endl;;
		};
		auto modern_approach = []()->void{
			std::vector<int> data = {1, 2, 3, 4, 5};
    
			if (size_t n = data.size(); n > 10) {  // 初始化 + 条件
				std::cout << "Large vector: " << n << std::endl;
			} else {
				std::cout << "Small vector: " << n << std::endl;
			}
			 // n 不再可见！
			// std::cout << n;  // 编译错误
		};
		traditional_approach();
		modern_approach();
    }
    //指针和智能指针示例
    {
		class MyClass{
		public:
			~MyClass(){
				std::cout<<"unique ptr delete auto!\n";
			}
			void doSomething() { std::cout << "Doing something\n"; }
			int getValue() const { return 42; }
		};
		//自动销毁的指针包裹器unique_ptr，用 make_unique 创建
		//引用了外面的函数，不用引用捕获
		auto getExpensiveResource = []()->std::unique_ptr<MyClass>{
			if(auto result=expensive_operation(200);result != std::nullopt )
			{
				return std::make_unique<MyClass>();
			}else
			{
				return nullptr;
			}
		};
		//调用了local的lambda 要捕获
		auto traditional_approach = [&]()->void{
			// 或者更复杂的获取过程
			auto result = getExpensiveResource();
			
			if (result != nullptr) {
				result->doSomething();
				std::cout << "Value: " << result->getValue() << std::endl;
			} else {
				std::cout << "No resource available\n";
			}
		};
		//调用了local的lambda 要捕获
		auto modern_approach = [&]()->void{
			if (auto result = getExpensiveResource(); result != nullptr) {
				result->doSomething();
				std::cout << "Value: " << result->getValue() << std::endl;
			} else {
				std::cout << "No resource available\n";
			}
			// result 不可见
		};
		traditional_approach();
		modern_approach();
		
    }
    //容器查找
    {
		std::map<std::string, int> dict = {
			{"apple", 1}, {"banana", 2}, {"orange", 3}
		};
		std::string key = "banana";
		auto traditional_approach = [&]()->void{
			auto it = dict.find(key);
			if (it != dict.end()) {
				std::cout << key << " found with value: " << it->second << std::endl;
			} else {
				std::cout << key << " not found\n";
			}
			// it 仍然可见，可能被误用
		};
		auto modern_approach = [&]()->void{
			// 现代写法 - 推荐！
			if (auto it = dict.find(key); it != dict.end()) {
				std::cout << key << " found with value: " << it->second << std::endl;
			} else {
				std::cout << key << " not found\n";
			}
			// it 不可见，避免了意外使用
		};
		traditional_approach();
		modern_approach();
    }
    //多个初始化变量，直接形式也可以
    {
		// 生成随机数
		std::mt19937 gen(std::random_device{}());
		std::uniform_int_distribution<> dist(1, 100);
		// 传统写法
		{
			int random_value = dist(gen);
			bool is_even = (random_value % 2 != 0);
			bool is_large = (random_value > 50);
			
			if (is_odd && is_large) {
				std::cout << "Large even number: " << random_value << std::endl;
			}
		}
		{
			// 带初始化的 if 写法（多个条件），c++20 也编译不过，c++用make_tuple的方式
			//为什么强调这个，多个独立的变量才是重要的，实际上考虑这种情况的才符合条件，否则判断语句直接移动掉条件判断就可以了
			if (auto [value, in_range, is_even] = [&]() {
					int v = dist(gen);
					return std::make_tuple(v, 
					(v>50),
					(v % 2 != 0));
				}();
				is_even && in_range) {
				std::cout << "Large even number: " << value << std::endl;
			}
		}
		{
			// 或者更简洁的写法
			if (int val = dist(gen); val % 2 != 0 && val > 50) {
				std::cout << "Large even number: " << val << std::endl;
			}
		}
    }
    //多个初始化变量，用tuple的形式，或者嵌套的形式
    {
		std::vector<int> vec = {1, 2, 3, 4, 5};
		vec.reserve(8);//不是捕获问题，是capacity和size调用API的情况，不是逐步push_back，两者保持相等
		auto modern_approach_tuple = [&]()->void{
			// 正确的多变量初始化
			if (auto&& [size, capacity] = std::make_pair(vec.size(), vec.capacity()); 
				size > 0 && size < capacity) {
				std::cout << "Size: " << size << ", Capacity: " << capacity << std::endl;
			}
		};
		auto modern_approach_NestIf = [&]()->void{
			// 或者使用结构化绑定（更常见）
			if (auto size = vec.size(); size > 0) {
				if (auto capacity = vec.capacity(); capacity > size) {
					std::cout << "Has room: " << size << "/" << capacity << std::endl;
				}
			}
		};
		modern_approach_tuple();//same life time
		modern_approach_NestIf();
    }
    //如果函数返回的值类型是std::optional<T> 就有has_value的判断，现在c#的nullable type也差不多这个用法
    {
		//直接调用expensive_operation 它返回一个optional
		{//传统写法
			auto opt_result = expensive_operation(500);
			if (opt_result.has_value()) {
				std::cout << "Got value: " << opt_result.value() << std::endl;
			} else {
				std::cout << "No value\n";
			}
		}
		// 现代写法
		if (auto opt_result = expensive_operation(300); opt_result.has_value()) {
			std::cout << "Got value: " << opt_result.value() << std::endl;
		} else {
			std::cout << "No value\n";
		}
    }
    //性能相关，非必要不创建，使用范围非常窄，在if-else语句中 自动释放了。感觉跟//指针和智能指针示例 相同，只是重点不同
    {
		class ExpensiveObject {
		public:
			ExpensiveObject() { 
				std::cout << "Expensive object created\n"; 
			}
			~ExpensiveObject() { 
				std::cout << "Expensive object destroyed\n"; 
			}
			bool isValid() const { return true; }
			int getValue() const { return 42; }
			// 禁止复制，只允许移动
			ExpensiveObject(const ExpensiveObject&) = delete;
			ExpensiveObject& operator=(const ExpensiveObject&) = delete;
			ExpensiveObject(ExpensiveObject&&) = default;
			ExpensiveObject& operator=(ExpensiveObject&&) = default;
		};
		//因为引用了外面的函数，所以需要设置捕获模式，这里设置为&引用
		auto getExpensiveResource = []()->std::optional<std::tuple<ExpensiveObject,int>>{
			if(auto result=expensive_operation(200);result != std::nullopt )
			{
				// 使用std::make_tuple，并移动构造ExpensiveObject
				return std::make_tuple(ExpensiveObject{},*result);
			}else
			{
				return std::nullopt;
			}
		};
		// 传统写法
		{
			auto opt_result = getExpensiveResource();
			if (opt_result.has_value()) {
				auto [x,y] = std::move(*opt_result);
				std::cout << "Create ExpensiveObject "<<"\t:"<<"Got value: " <<y << std::endl;
			} else {
				std::cout << "No value\n";
			}
		}
		{
			// 现代写法
			if (auto opt_result = getExpensiveResource(); opt_result.has_value()) {
				if( auto& [x,y] = *opt_result;y != 0){
					std::cout << "Create ExpensiveObject "<<"\t:"<<"Got value: " <<y << std::endl;
				}
			} else {
				std::cout << "No value\n";
			}
		}
    }
    //dynamic_cast 和类型检查
    {
		class Base {
		public:
			virtual ~Base() = default;
		};

		class Derived : public Base {
		public:
			void special_method() { std::cout << "Derived method" << std::endl; }
		};
		// 传统写法
		{
			Base* base_ptr = new Derived();
			Derived* derived = dynamic_cast<Derived*>(base_ptr);
			if (derived) {
				derived->special_method();
			}
			// derived 在整个块内可见，可能为 nullptr
			delete base_ptr;
		}
		{
			Base* base_ptr = new Derived();
			// 带初始化的 if 写法
			if (Derived* derived = dynamic_cast<Derived*>(base_ptr); derived) {
				derived->special_method();
			}
			delete base_ptr;
		}
    }
    return 0;
}
//cl /EHsc /std:c++17 20PythonWalrusOpLike.cpp /link psapi.lib 