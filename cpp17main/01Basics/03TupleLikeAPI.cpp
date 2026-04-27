#include <iostream>
#include <vector>
#include "../TestMacro.h"
#include <utility> //for std::move
#include <string> //for class object
/**
 * @file 03TupleLikeAPI.cpp
 * @ingroup 01Basics
 * @brief 为自定义类实现 tuple-like API — 结构化绑定的完整支持
 * @details 演示如何通过特化 `std::tuple_size`、`std::tuple_element` 和
 *          自定义 `get<I>()` 函数，使自定义类支持 C++17 结构化绑定。
 *          展示了两种风格：
 *          - Customer：传统的逐个特化 get 函数
 *          - Customer2：使用 `if constexpr` + `decltype(auto)` + `if` 编译期分支的更现代写法
 */

class Customer{
private:
	std::string first;
	std::string last;
	long 		val;
public:
	//多种形式的parameter initialization 过程
	//注意有的有copy，有的没有，对象要看赋值语句的含义
	Customer(std::string f, std::string l,long v)
			:first{std::move(f)},last{l},val(v) 
			{
			}
	//自动推理出来的类型
	auto getFirst() const{
		return first;
	}
	std::string getLast()const{
		return last;
	}
	long getValue() const{
		return val;
	}
};
// 1：书上提供的类型，应该在cpp17特有的？
// 为类Customer提供tuple-like API来支持结构化绑定
template<>
struct std::tuple_size<Customer>{
	static constexpr int value = 3;//有三个属性
};
template<>
struct std::tuple_element<2,Customer>{
	using type = long;//第三个属性的类型是long
};
template<std::size_t Idx>
struct std::tuple_element<Idx,Customer>{
	using type = std::string;//其他属性都是string
};
// 定义特化的getter：注意这个必须在Customer同级的命名空间中定义。
template<std::size_t> auto get(const Customer& c);
template<> auto get<0>(const Customer& c) { return c.getFirst(); }
template<> auto get<1>(const Customer& c) { return c.getLast(); }
template<> auto get<2>(const Customer& c) { return c.getValue(); }

// 定义相同的类型但不同名，展示另外一种用法，比如不用auto，使用**编译器if语句特性**等
// **c++17**就有了，不用升级到c++20的
class Customer2{
private:
	std::string first;
	std::string last;
	long 		val;
public:
	//多种形式的parameter initialization 过程
	//注意有的有copy，有的没有，对象要看赋值语句的含义
	Customer2(std::string f, std::string l,long v)
			:first{std::move(f)},last{std::move(l)},val{v} 
			{
			}
	const std::string& firstname() const{
		return first;
	}
	std::string& firstname(){
		return first;
	}
	const std::string& lastname() const{
		return last;
	}
	std::string& lastname(){
		return last;
	}
	long value() const{
		return val;
	}
	long& value(){
		return val;
	}
};
//注意decltype(auto)需要这些信息 就是tuple_size和tuple_element
//使用if 编译选项只是减少get语句，不过通过传入参数的形式来看，能减少不少的，棒！！！
template<>
struct std::tuple_size<Customer2>{
	static constexpr int value = 3;//有三个属性
};
template<>
struct std::tuple_element<2,Customer2>{
	using type = long;//第三个属性的类型是long
};
template<std::size_t Idx>
struct std::tuple_element<Idx,Customer2>{
	using type = std::string;//其他属性都是string
};
//为这个新类型支持特化的getter，注意类型不同，要用decltype(auto) 对应不同方式就有不用返回了
//用了这种方式，就不需要像上面那么定义一堆的get形式了。
//参数不同的形式。对应常量对象、非常量对象、可移动对象
template<std::size_t I> decltype(auto) get(const Customer2& c){
	static_assert(I<3);
	if constexpr (I==0) {
		return c.firstname();
	}
	else if constexpr(I==1){
		return c.lastname();
	}else { //I==2
		return c.value();
	}
}
//非常量对象
template<std::size_t I> decltype(auto) get(Customer2& c){
	static_assert(I<3);
	if constexpr (I==0) {
		return c.firstname();
	}
	else if constexpr(I==1){
		return c.lastname();
	}else { //I==2
		return c.value();
	}
}
//可移动对象
template<std::size_t I> decltype(auto) get(Customer2&& c){
	static_assert(I<3);
	if constexpr (I==0) {
		return std::move(c.firstname());
	}
	else if constexpr(I==1){
		return std::move(c.lastname());
	}else { //I==2
		return c.value();
	}
}
int main() {
	// added the two lines below
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(NULL);
	{
		Customer c{"Tim", "Starr", 42};
		auto [f,l,v] = c;
		std::cout << "f/l/v:    " << f << ' ' << l << ' ' << v << '\n';
		//f/l/v:    Tim Starr 42
		// 修改结构化绑定的变量
		std::string s{std::move(f)};
		l = "Waters";
		v += 10;
		std::cout << "f/l/v:    " << f << ' ' << l << ' ' << v << '\n';
		//f因为使用了move，所以为empty string了
		//f/l/v:     Waters 52
		//class 还是包含解析前的内容，所以，这里输出的内容还是全面的。
		std::cout << "c:        " << c.getFirst() << ' '
		<< c.getLast() << ' ' << c.getValue() << '\n';
		//c:        Tim Starr 42
		//s 接受了 解析f的内容，应该就是默认值 Tim
		std::cout << "s:        " << s << '\n';
		//s:        Tim
	}
	{
		Customer2 c{"Tim", "Starr", 42};
		auto [f,l,v] = c;
		std::cout << "f/l/v:    " << f << ' ' << l << ' ' << v << '\n';
		//f/l/v:    Tim Starr 42
		
		//通过引用修改结构化绑定，对应move
		auto&& [f2, l2, v2] = c;
		std::string s{std::move(f2)};
		f2 = "Ringo";
		v2 += 10;
		std::cout << "f2/l2/v2: " << f2 << ' ' << l2 << ' ' << v2 << '\n';
		//f因为使用了move，所以为empty string了
		//f/l/v:     Waters 52
		//class 还是包含解析前的内容，所以，这里输出的内容还是全面的。
		std::cout << "c:        " << c.firstname() << ' '
		<< c.lastname() << ' ' << c.value() << '\n';
		//c:        Tim Starr 42
		//s 接受了 解析f的内容，应该就是默认值 Tim
		std::cout << "s:        " << s << '\n';
		//s:        Tim
	}
    return 0;
}
//cl /EHsc /std:c++17 03TupleLikeAPI.cpp /link psapi.lib 