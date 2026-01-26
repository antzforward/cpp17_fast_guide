#include <iostream>
#include <string>
#include <memory>
void foo(std::shared_ptr<int> i)
{
	(*i)++;
}
//当出现环状缠绕结构时，shared_ptr也会出现问题
struct A;
struct B;
struct A {
	std::shared_ptr<B> pointer;
	void foo(){
		std::cout<<"A::foo()"<<std::endl;
	}
	~A() {
		std::cout << "A 被销毁" << std::endl;
	}
};
struct B {
	std::weak_ptr<A> pointer;//可以用weak，操作的时候转成shared就好了。
	~B() {
		std::cout << "B 被销毁" << std::endl;
	}
};
int main()
{
	// auto pointer = new int(10); // 非法, 不允许直接赋值
	// 构造了一个 std::shared_ptr
	auto pointer = std::make_shared<int>(10);
	foo(pointer);
	std::cout << *pointer << std::endl; // 11
	
	auto pointer2 = pointer; // 引用计数+1
	auto pointer3 = pointer; // 引用计数+1
	int *p = pointer.get(); // 这样不会增加引用计数
	std::cout << "pointer.use_count() = " << pointer.use_count() <<	std::endl; // 3
	std::cout << "pointer2.use_count() = " << pointer2.use_count() << std::endl; // 3
	std::cout << "pointer3.use_count() = " << pointer3.use_count() << std::endl; // 3
	pointer2.reset();
	std::cout << "reset pointer2:" << std::endl;
	std::cout << "pointer.use_count() = " << pointer.use_count() <<	std::endl; // 2
	std::cout << "pointer2.use_count() = " << pointer2.use_count() << std::endl; // 0, pointer2 已 reset
	std::cout << "pointer3.use_count() = " << pointer3.use_count() << std::endl; // 2
	pointer3.reset();
	std::cout << "reset pointer3:" << std::endl;
	std::cout << "pointer.use_count() = " << pointer.use_count() <<	std::endl; // 1
	std::cout << "pointer2.use_count() = " << pointer2.use_count() << std::endl; // 0
	std::cout << "pointer3.use_count() = " << pointer3.use_count() << std::endl; // 0, pointer3 已 reset
	
	//会出现不释放的情况。传说中左脚踩右脚 右脚踩左脚的效果。
	auto a = std::make_shared<A>();
	auto b = std::make_shared<B>();
	a->pointer = b;
	b->pointer = a;
	//转换一下再访问
	if( auto sharedPointerToA = b->pointer.lock()){
		// Use sharedPointerToA, which is a std::shared_ptr<A>, inside this block
        // When this block exits, sharedPointerToA will be destroyed, potentially
        // allowing A to be destroyed, if no other std::shared_ptr instances exist.
        sharedPointerToA->foo();
	}
	// 离开作用域前，shared_ptr 会被析构，从而释放内存
	return 0;
}
