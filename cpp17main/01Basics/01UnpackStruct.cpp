#include <vector>
#include <string>
#include <iomanip>
#include "../TestMacro.h"


struct MyStruct{
	int i;
	std::string s;
	double c;
	MyStruct(int intV,std::string strV,double doubleV){
		i=intV;
		s = strV;
		c = doubleV;
	}
	MyStruct(int intV,std::string strV):MyStruct(intV,strV,3.1415926535)
	{
	}
	MyStruct(int intV):MyStruct(intV,"Empty Boy!"){
	}
	MyStruct():MyStruct(0){}
};
struct OtherStruct{
	int id;
	int value;
	double other;
	std::string otherInfo;
	void PrintOtherStruct(){
		std::cout<<"id："<<id<<"		a："<<value<<"		b："<<std::setprecision(12)<<other<<" hint: "<<otherInfo<<std::endl;
	}
};

MyStruct getStruct(){
	return MyStruct{42,"Hello World",0.001};
}
int main() {
	// added the two lines below
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(NULL); 
	///CODE:struct MyStruct{
	///CODE:	int i;
	///CODE:	std::string s;
	///CODE:	double c;
	///CODE: }
	///CODE:MyStruct ms;
	///Not:这种写法已经会出现编译错误了，因为语法跟initializer list太一致了，出现二义性。
	///Code:auto [u2, v2] {ms}; 
	///Note:下面这个形式还是可以的。
	///CODE：auto [u3, v3] (ms);
	///Note：通常解析的语法
	///Code：auto [u,v,w]=ms;
	{
		MyStruct ms;
		auto [u,v,w]=ms;
		std::cout<<"MyStruct.i："<<u<<"		MyStruct.s："<<v<<"		MyStruct.c："<<std::setprecision(12)<<w<<std::endl;
	}
	///Note: python中近似写
	///Code(Python): p=(42,"Bad Boy!")
	///Code(Python): u1,v1=p
	{
		auto [u,v,w]= MyStruct{42,"Bad Boy!"};
		std::cout<<"MyStruct.i："<<u<<"		MyStruct.s："<<v<<"		MyStruct.c："<<std::setprecision(12)<<w<<std::endl;
	}
	///NOTE：共用解析临时变量,实际上不行的,不能复用临时变量
	///auto [u1,v1,w1]= (ms);
	///NOTE：修改解析后的内容，看是否影响struct自己
	///auto [u2,v2,w2] = ms2;
	///u2=13;v2="Fixed strz";w2 = 1.23456789;
	///std::cout<<"MyStruct.i："<<ms2.i<<"		MyStruct.s："<<ms2.s<<"		MyStruct.c："<<std::setprecision(12)<<ms2.c<<std::endl;
	///MyStruct.i：42          MyStruct.s：Hello World         MyStruct.c：0.001
	{
		auto ms2 = getStruct();
		auto [u2,v2,w2] = ms2;
		std::cout<<"MyStruct.i："<<u2<<"		MyStruct.s："<<v2<<"		MyStruct.c："<<std::setprecision(12)<<w2<<std::endl;
		u2=13;
		v2="Fixed strz";
		w2 = 1.23456789;
		std::cout<<"MyStruct.i："<<ms2.i<<"		MyStruct.s："<<ms2.s<<"		MyStruct.c："<<std::setprecision(12)<<ms2.c<<std::endl;
	}
	///NOTE：用引用分解的方式,如果要是imutable的用const auto&的形式
	///auto& [u3,v3,w3] = ms2;
	///u3=13;v3="Fixed strz";w3 = 1.23456789;
	///std::cout<<"MyStruct.i："<<ms2.i<<"		MyStruct.s："<<ms2.s<<"		MyStruct.c："<<std::setprecision(12)<<ms2.c<<std::endl;
	///MyStruct.i：13          MyStruct.s：Fixed strz          MyStruct.c：1.23456789
	{
		auto ms2 = getStruct();
		auto& [u3,v3,w3] = ms2;
		std::cout<<"MyStruct.i："<<u3<<"		MyStruct.s："<<v3<<"		MyStruct.c："<<std::setprecision(12)<<w3<<std::endl;
		u3=13;
		v3="Fixed strz";
		w3 = 1.23456789;
		std::cout<<"MyStruct.i："<<ms2.i<<"		MyStruct.s："<<ms2.s<<"		MyStruct.c："<<std::setprecision(12)<<ms2.c<<std::endl;
		
		const auto& [u4,v4,w4] = ms2;
		std::cout<<"MyStruct.i："<<u4<<"		MyStruct.s："<<v4<<"		MyStruct.c："<<std::setprecision(12)<<w4<<std::endl;

	
		// const auto 变量不能赋值。
		//v4="fix again";
		// 但是指向的结构体还是可以赋值修改的
		ms2.s="fix again";
		std::cout<<"MyStruct.i："<<u4<<"		MyStruct.s："<<v4<<"		MyStruct.c："<<std::setprecision(12)<<w4<<std::endl;
		//MyStruct.i：13          MyStruct.s：fix again           MyStruct.c：1.23456789
	}
	
	/// c++不容许下面这种混合模式，部分引用，部分赋值，编译不通过
	//auto [u5,&v5,w5] = ms2;
	//std::cout<<"MyStruct.i："<<u5<<"		MyStruct.s："<<v5<<"		MyStruct.c："<<std::setprecision(12)<<w5<<std::endl;
    
    //move语义
    {
		auto ms = MyStruct{41};
		auto&& [u,v,w]=std::move(ms);
		std::cout<<"MyStruct.i："<<u<<"		MyStruct.s："<<v<<"		MyStruct.c："<<std::setprecision(12)<<w<<std::endl;
		//MyStruct.i：41          MyStruct.s：Empty Boy!          MyStruct.c：3.1415926535
		auto s = std::move(v);
		v = "Lara";
		std::cout<<"MyStruct.s："<<ms.s<<"		unpack v："<<v << "		move s:"<<s<<std::endl;
		//MyStruct.s：Lara                unpack v：Lara          move s:Empty Boy!
    }
    // unpack 接受体不是auto &&,是auto，代表直接完成接受
    {
		auto ms = MyStruct{41};
		auto [u,v,w]=std::move(ms);
		std::cout<<"MyStruct.i："<<ms.i<<"		MyStruct.s："<<ms.s<<"		MyStruct.c："<<std::setprecision(12)<<ms.c<<std::endl;
		//MyStruct.i：41          MyStruct.s：            MyStruct.c：3.1415926535
		std::cout<<"MyStruct.i："<<u<<"		MyStruct.s："<<v<<"		MyStruct.c："<<std::setprecision(12)<<w<<std::endl;
		//MyStruct.i：41          MyStruct.s：Empty Boy!          MyStruct.c：3.1415926535
    }    
    return 0;
}
//cl /EHsc /std:c++17 01UnpackStruct.cpp /link psapi.lib 