#include <iostream>
#include <string>
#include <vector>
using namespace std;
template<typename T, typename U>
class SuckType{};

//typedef SuckType<vector, string> NewType;//不合法。
//using NewType = SuckType<vector,string>;//vector 缺少T定义，也不行。
template<typename ElementT > 
using NewType = SuckType<vector<ElementT>,string>;
//typedef 用法比较多。现在能用在什么情况？
//定义一个返回类型为int，参数为 int int的的函数指针，名字叫process
typedef int (* processfn ) (int,int );//*就可以定义指针类型了，塞进去的processfn就是命名。
int add(int,int);
int sub(int a,int b);
int multiply( int a, int b);
int divide( int a, int b);
using processFN = int(*)(int, int);
int main(){
	processfn myFunctionPointer = add;
	cout<<"add( 10, 100): "<<myFunctionPointer(10, 100)<<endl;
	processfn pointers[]={add, sub, multiply, divide};
	for(int i=0;i<4;i++){
		cout<<"?( 10, 100): "<<pointers[i](10, 100)<<endl;
	}
	
	processFN fnPointer = add;
	cout<<"add( 10, 100): "<<fnPointer(10, 100)<<endl;
	processFN fnpointers[]={add, sub, multiply, divide};
	for(int i=0;i<4;i++){
		cout<<"?( 10, 100): "<<fnpointers[i](10, 100)<<endl;
	}
}
int add(int a, int b){
	return a + b;
}
int sub(int a,int b){
	return a - b;
}
int multiply( int a, int b){
	return a * b;
}

int divide( int a, int b){
	return a / b;
}