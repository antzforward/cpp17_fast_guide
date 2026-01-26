#include <iostream>
using namespace std;
//lambda 表达式提供一个方便的方式来定义匿名函数对象，基本语法如下,不如c#，rust之类简洁。
//[capture_clause]( parameters )->return_type{
//	function_body
//}
//1、capture_clause：捕捉列表，决定了外部作用域中哪些变量被lambda 表达式捕获，以及如何捕获（值捕获或者引用捕获）
//2、parameters：参数列表，类似于普通函数的参数列表
//3、return_type:返回类型，可选项，若未指定，且函数体内只有单一返回语句，编译器会自动推导返回类型，
//4、function_body:代码块。
//无参数和返回值的lambda
auto greet = [](){cout<<"Hello World!"<<endl;};
//带参数的lambda，返回值类型自动推定
auto add =[](int a, int b){return a+b;};
//带参数和指定返回值的lambda
auto divide = [](double a, double b )->double {
	if(b == 0.0) throw std::runtime_error("Division by zero.");
	return a/b;
};

int main()
{
	greet();
	cout<<add(3,4)<<endl;
	cout<<divide(10.0,2.0)<<endl;
	
	//捕捉要设定环境，直接放外面是不行的 scope 不确定。
	int x=10;
	auto add_to_x =[x](int y) {return x+y;};
	auto add_to_x_ref = [&x](int y){return x+y;};
	x = add_to_x(5);
	cout<<x<<endl;
	x = 10;
	x = add_to_x_ref(5);
	cout<<x<<endl;
}
//捕获有四种形式
//1、[]空捕获列表，纯粹的函数形式
//2、[name1,&name2,...]捕获一系列变量。每个变量指定捕获模式。
//3、[&]引用捕获，让编译器自行推导捕获列表，列表中所有变量都是引用。
//4、[=]值引用，让编译器执行推导应用列表，最终列表中所有变量都是值引用（注意string，vector等）