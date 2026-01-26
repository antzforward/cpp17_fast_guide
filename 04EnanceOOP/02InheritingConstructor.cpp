#include <iostream>
using namespace std;

class Base{
public:
	int value1;
	int value2;
	Base(){
		value1= 1;
	}
	Base(int value):Base(){
		value2 = 2;
	}
	int GetValue3(){
		return value3;
	}
	void SetValue3(int v){
		value3 = v;
	}
private:
	int value3{0};
};

class Subclass:public Base {
	public:
		using Base::Base;//inherite constructor,
		//其实是把Base的部分都放public下面了？反正下面用s.GetValue3()是可以的。不能越过Base的权限，s.value3是不行的。
};

int main(){
	Subclass s(3);
	cout<<s.value1<<" , "<<s.value2<<", "<<s.GetValue3()<<endl;
}