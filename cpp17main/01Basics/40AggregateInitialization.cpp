#include <iostream>
#include <vector>
#include "../TestMacro.h"
#include <string>
#include <complex>
/**
 聚合体初始化的改进
 聚合体可以认为是纯数据类型的struct，可以通过继承的关系逐渐扩展数据类型
 感觉跟ECS中E的定义很接近。
 之前在测试unpack 继承的struct 就发现不好解析，也就是自己要补充tuple_size/tuple_element 这些stl函数
 如果要序号访问，就要自己实现get了，总体来说还是麻烦一点
 具体可以看“03TupleLikeInheritance.cpp”跟扩展
 但是初始化的过程，就相对比较简单了。
 我这里还是用上面的struct形式来定义了，不用书上给的例子，too simple！！！
*/
/**
总的来说，在C++17中满足如下条件之一的对象被认为是聚合体：
• 是一个数组
• 或者是一个满足如下条件的类类型(class、struct、union)：
	– 没有用户定义的和explicit的构造函数
	– 没有使用using声明继承的构造函数
	– 没有private和protected的非静态数据成员
	– 没有virtual函数
	– 没有virtual, private, protected的基类
然而，要想使用聚合体初始化来初始化聚合体，那么还需要满足如下额外的约束：
• 基类中没有private或者protected的成员
• 没有private或者protected的构造函数
这里可以检查一下：
C++17引入了一个新的类型特征is_aggregate<>来测试一个类型是否是聚合体
*/
/// 一种没有意义，只是展示难度的聚合体形式,没有用户定义的构造函数，没有私有或保护的非静态数据成语，没有虚函数
/// 在c++17之前，还要求没有继承。其实就是python中的named tuple形式吧。
struct A{
	float a1,a2=1.0f;//可以设置默认值
	void print(){
		std::cout<<"A.a1:"<<a1<<"	A.a2:"<<a2<<'\n';
	}
};
struct B:A{
	double b1=3.1415926,b2;
	void print(){
		A::print();//c# 的形式接近这个
		//static_cast<A*>(this)->print();//这种也可以的
		//std::cout<<"A.a1:"<<a1<<"	A.a2:"<<a2<<'\n';//这种也可以的
		std::cout<<"B.b1:"<<b1<<"	B.b2:"<<b2<<'\n';
	}
};
struct C:B{
	std::string c1,c2,c3;//也可以都不设置的
	void print(){
		B::print();
		std::cout<<"C.c1:"<<c1<<"	C.c2:"<<c2<<"	C.c3:"<<c3<<'\n';
	}
};
struct D:C{
	std::vector<int> d1;
	std::string d2;
	void print(){
		C::print();
	}
};
//// 继承std::string=>可以从非聚合体派生出聚合体
struct MyString:std::string {
	void print() const {
		if (empty()) {
			std::cout << "<undefined>\n";
		}
		else
		{
			std::cout << c_str() << '\n';			
		}
	}
};
//// 可以从多个基类和聚合体中派生出聚合体，确实聚合体支持多继承形式是最合适的，pure data嘛
template<typename T>
struct E:std::string,std::complex<T>
{
	//继承体增加的额外信息
	//继承且包含
	std::string data;
	//data 排序是，string，complex<T>,string data
	//自己补充一个print方法
	void print()const{
		const std::string base1 = static_cast<const std::string&>(*this);
		if (base1.empty()) {
			std::cout << "<undefined>";
		}
		else
		{
			std::cout << base1.c_str();			
		}
		const std::complex<T>& base2 = static_cast<const std::complex<T>&>(*this);
		std::cout << '\t' << base2.real() << "\t," << base2.imag() << '\t';
        std::cout << data << '\n';
	}
	// 可选：添加一个更好的输出格式化
    void print_formatted() const {
        const std::string& str_base = static_cast<const std::string&>(*this);
        const std::complex<T>& complex_base = static_cast<const std::complex<T>&>(*this);
        
        std::cout << "String: ";
        if (str_base.empty()) {
            std::cout << "<empty>";
        } else {
            std::cout << '\"' << str_base << '\"';
        }
        
        std::cout << " | Complex: (" << complex_base.real() 
                  << ", " << complex_base.imag() << "i)";
        std::cout << " | Data: \"" << data << "\"\n";
    }
};
//// c++17 增加了一个新的类型特征 is_aggregate<>来测试一个类型是否是聚合体
//// c++17 满足如下条件之一的对象被认为是聚合体
//// 1. 一个数组
//// 2. 满足如下条件的类类型（class,struct, union）:
//// 	- 没有用户定义的和explicit的构造函数
//// 	- 没有使用using声明继承的构造函数
//// 	-  没有private 和protected 的非静态数据成员
//// 	-  没有virtual 函数
//// 	-  没有virtual private，protected的机累。
int main() {
	// added the two lines below
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(NULL);  
	// 示例1，
    {
		//按照顺序初始化。
		A a1={1.f,2.f};
		a1.print();//A.a1:1  A.a2:2
		A a2{1.f,2.f};//c++11之后可以忽略=
		A a3{};//默认初始化，未设置值的，都给设置为default
		a3.print();//A.a1:0  A.a2:1
		A a4;//成员的值未定义
		a4.print();//A.a1:9.33281e+09        A.a2:1
		B b1{{1.f,2.f},6.11,7.11};
		B b2{1.f,2.f,6.11,7.11};//c++17之后可以继承，并且初始化过程不用加{}
		b2.print();
		//A.a1:1  A.a2:2
		//B.b1:6.11       B.b2:7.11
		B b3{{3.f},6.11};//一旦初始化，没有特别设置的就是default
		b3.print();
		//A.a1:3  A.a2:1
		//B.b1:6.11       B.b2:0
		
    }
    //非聚合体派生出聚合体。核心时std::string没有虚成员函数，因此这里就没有virtual table了
    {
		MyString str1{};
		str1.print();
		MyString str2={"Hello World!"};
		str2.print();
		MyString str3;
		str3.print();
		MyString x{{"hello"}};
		MyString y{"world"};
		x.print();
		y.print();
    }
    //继承多个父类型组成新的聚合体的方式，嗯，可以说非常的有意思
    {
		E<float> s{{"hello"}, {4.5, 6.7}, "world"}; // 自从C++17起OK
		E<float> t{"hello", {4.5, 6.7}, "world"}; // 自从C++17起OK
		std::cout << s.data<<'\t';
		// 输出："world"
		std::cout << static_cast<std::string>(s)<<'\t'; // 输出："hello"
		std::cout << static_cast<std::complex<float>>(s)<<'\n'; //输出：(4.5,6.7)
		t.print();
		s.print_formatted();
		E<float> w{{}, {4.5, 6.7}, "pop"};
		// 自从C++17起OK
		std::cout << std::is_aggregate<decltype(w)>::value<<'\t'; // 输出1(true)
		w.print_formatted();
		//// 1       String: <empty> | Complex: (4.5, 6.7i) | Data: "pop"
    }
    return 0;
}
//cl /EHsc /std:c++17 40AggregateInitialization.cpp /link psapi.lib 