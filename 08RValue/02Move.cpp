#include <iostream>
//这个例子说明的内容很重要，以前如果把局部的对象作为return obj，直接编译就出错了。
//当然这里也有一点，局部对象包裹的还是堆上的内存。
class A {
public:
	int *pointer;
	A():pointer(new int(1)) { std::cout << "构造" << pointer << std::endl; }
	A(A& a):pointer(new int(*a.pointer)) { std::cout << "拷贝" <<pointer << std::endl; } // 无意义的对象拷贝
	A(A&& a):pointer(a.pointer) { a.pointer = nullptr;std::cout<< "移动" << pointer << std::endl; }
	~A(){ std::cout << "析构" << pointer << std::endl; delete pointer; }
};
// 防止编译器优化
A return_rvalue(bool test) {
	A a,b;
	if(test) return a;
	else return b;
}
int main() {
	A obj = return_rvalue(false);
	std::cout << "obj:" << std::endl;
	std::cout << obj.pointer << std::endl;
	std::cout << *obj.pointer << std::endl;
	return 0;
}
