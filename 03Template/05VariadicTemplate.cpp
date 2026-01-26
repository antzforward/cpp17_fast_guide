#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <initializer_list>
using namespace std;
//1. C++11 允许任意个数、任意类别的模板参数，同时不需要再定义时将参数个数固定。
template<typename... Ts> class Magic{};//要补全Magic的class定义，否则模板实现时就报错了。
class Magic<int,
			vector<int>,
			map<string,vector<int>>> darkMagic;// 如果能编译通过就是OK的，这个要注意extern或者预先定义，提高编译效率。
//参数个数为0的模板参数也是可以的。
class Magic<> nothing;
//2.不希望产生的模板参数为0，可以手动的定义至少一个模板参数，嗯，这个比较符合 n=1+ n-1，天然的递归写法。太简单的我不写了。
template<typename T>
void print(T value){
	cout<< value<<' ';
}
template<typename T, typename... Args>
void print(T value,Args... args) {
	print(value);//这里用printf 就会调用会默认的printf了。
	print(args...);
}
//3.typename... 的操作有 sizeof...;感觉没有讲完啊。
template<typename... Args>
void magic(Args... args){
	cout<<sizeof...(args)<<endl;
}
//4.初始化列表展开。initialize_list的用法，之前用过，也就是都支持{...}的初始化过程，这里是说，配合模板参数作为参数的写法。
template<typename T, typename... Args>
auto print2(T value, Args... args){
	cout<<value<<' ';
	return initializer_list<T>{([&]{
		cout<<args<<' ';
	}(),value)...	};
}
int main(){
	magic();
	magic(1);
	magic(1,"");
	
	print(1,2,"123",1.1);
	cout<<endl<<endl;
	print2(1,2,"123",1.1);
	return 0;
}