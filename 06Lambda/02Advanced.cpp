#include <iostream>
#include <string>
#include <limits>
#include <utility>

//1.泛型Lambda C++14起，用auto关键字定义参数类型，可以接收任何参数类型
auto genericLambda =[](auto x, auto y){return x+y;};
//2.捕获*this（自c++17起）可以通过值捕获当前对象（*this）来在Lambda内部安全地使用成员变量
class C{
private:
	std::string name;
public:
	void foo(){
		auto l1=[this]{std::cout<<name<<'\n';};
		auto l2=[=]{std::cout<<name<<'\n';};
		auto l3=[&]{std::cout<<name<<'\n';};
		l1();
		l2();
		l3();
	}
};
//3. constexpr Lambda :如果Lambda体满足constexpr函数的要求，那么这个Lambda可以在编译时进行计算
//显示或者隐式定义为constexpr，放前面后面不一样哦，这个是定义lambda本身是constexpr。
auto squared = [](auto val)constexpr{ return val*val;};
auto hashed = [](const char* str) constexpr {
	std::size_t hash = 5381;//初始化哈希值
	while(*str != '\0'){
		hash = hash*33^*str++;
	}
	return hash;
};
enum Hashed {//只验证能否在编译期确定，要遍历起来还是很麻烦啊。
	beer = hashed("beer"),
	wine = hashed("wine"),
	water = hashed("water"),
};
//更复杂的hashed写法，把 hash算法中替换函数。这个就知道怎么写Fibonacci的lambda写法了。
auto hashedCombine = [](const char* str,auto combine){
	std::size_t hash = 5381;//初始化哈希值
	while(*str != '\0'){
		hash = combine(hash,*str++);
	}
	return hash;
};
auto combine1=[](auto h, char c){return h*33+c;};
auto combine2=[](auto h, char c){return h*33^c;};
auto hash1=[](const char* str) constexpr {
	return hashedCombine(str,combine1);
};
auto hash2=[](const char* str) constexpr {
	return hashedCombine(str,combine2);
};

//4.捕获移动构造的对象，也就是在捕捉列表里面使用表达式的情况。对应的是[&&]的情况。
//5.异常规格和属性，添加noexcept之类的，比如divide 去掉throw，改成返回NaN的情况
auto divide = [](auto x, auto y) noexcept->decltype(x) {
	if( y == 0 ) return std::numeric_limits<decltype(x)>::quiet_NaN();
	return x/y;
};

//6.延伸捕获范围，在嵌套的Lambda中，可以显示地捕获上层作用域的变量，即便是在默认捕获模式下。


//7.指定返回类型，在一些复杂的情况，需要显式指明Lambda的返回类型。
auto complexReturnLambda = [](int x) -> std::pair<int, int> {
  if (x > 0)
    return {x, x};
  else
    return {-x, -x};
};

int main(){
	//6.示例
	int x = 10;
	auto outerLambda = [x]()mutable{
		return [&,x]{std::cout<<x<<std::endl;};//“x”: 无法在非可变 lambda 中修改通过复制捕获,其实没啥用啊
	};
	auto innerLambda = outerLambda(); // 调用外部lambda获取内部lambda。
	innerLambda(); // 调用内部lambda，增加外部lambda捕获的x的副本
	
	auto important = std::make_unique<int>(1);
	auto add = [v1 = 1, v2 = std::move(important)](int x, int y)-> int {
		return x+y+v1+(*v2);
	};
	std::cout << add(3,4) << std::endl;
	return 0;
}