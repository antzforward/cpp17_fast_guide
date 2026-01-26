#include <iostream>
using namespace std;

class Base{
public:
	int value1{0};//预设初始值的方式。
	int value2{0};
	Base()=default;//default标识属性非初始化，为任意值。等同于空
	Base(int value):Base(){
		value2 = value;
	}
};

class Cell{
public:
	int x{0};
	int y{0};
	
	Cell() = default;
	
	Cell(int value):Cell(){
		y = value;
	}
};
int main(){
	Base b(2);
	cout<<b.value1<<" ,"<<b.value2<<endl;
	Cell c(3);
	cout<<c.x<<" ,"<<c.y<<endl;
}
