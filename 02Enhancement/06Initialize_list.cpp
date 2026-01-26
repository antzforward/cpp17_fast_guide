#include <iostream>
#include <initializer_list>
#include <vector>
using namespace std;
//有{}初始化，有name()的初始化，那能不能统一，可以 initializer_list 
class Foo{
private:
	int value;
public:
	Foo(int i){ value = i;}
};
class Magic{
public:
	Magic(initializer_list<int> list){
		cout<<"initializer_list<int> list: {";
		for(auto &x:list) cout<<x<<",";
		cout<<"}"<<endl;
	}
};
struct A{
	int a;
	float b;
};
struct B {
	B(int _a,float _b):a(_a),b(_b){};
private:
	int a;
	float b;
};

struct C:A{
	int c;
};
struct D:B{
	int d;
};
void fee( initializer_list<int> list ){};

int main(){
	int a[] = {1,2,3,4,5};//list
	Foo foo(1);
	Magic magic = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18};
	vector<int> v = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18};
	fee(  {1,2,3,4,5} );
	
	A insta{1,1.1};
	B instb{2,2.2};
	C instc{3,3.3f,3};
	D instd{{4,4.4f},4};//因为B的private成员，并且自定义了带参数构造函数，因此，这里要套两个的格式不同
	return 0;
}