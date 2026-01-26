#include <iostream>
using namespace std;

struct Base{
	virtual void foo(int);
	virtual void boo(int);
};

struct SubClass:Base{
	virtual void foo( int ) override;//函数的override
};
struct SubClass1:Base{
	virtual void foo( int ) final;//函数的final，此函数不能再override了。
	virtual void boo( int ) override;
};
struct SubSubClass1:SubClass{
	//virtual void foo( int ) override; 不能再声明了。
	virtual void boo( int ) final;
};

struct SubClass2 final:SubClass1{ //不能再继承SubClass2了 它没有子类了。
	virtual void boo( int ) final;
};

void Base::foo(int v){cout<<"Base::foo(v)"<<v<<endl;}
void Base::boo(int v){cout<<"Base::boo(v)"<<v<<endl;}

void SubClass::foo(int v){cout<<"SubClass::foo(v)"<<v<<endl;}

void SubClass1::foo(int v){cout<<"SubClass1::foo(v) final"<<v<<endl;}
void SubClass1::boo(int v){cout<<"SubClass1::boo(v)"<<v<<endl;}

void SubSubClass1::boo(int v){cout<<"SubSubClass1::boo(v)"<<v<<endl;}
void SubClass2::boo(int v){cout<<"SubClass2::boo(v)"<<v<<endl;}

int main(){
	Base b;
	b.foo(1);
	b.boo(1);
	
	SubClass s;
	s.foo(2);
	s.boo(2);
	
	SubClass1 s1;
	s1.foo(3);
	s1.boo(3);
	
	SubSubClass1 s2;
	s1.foo(4);
	s1.boo(4);
	
	SubClass2 s3;
	s3.foo(4);
	s3.boo(4);
}