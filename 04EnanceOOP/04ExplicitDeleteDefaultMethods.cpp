#include <iostream>
#include <cstdlib> 
#include <new>
using namespace std;
//传统cpp中默认提供的函数有，默认构造函数，复制构造，赋值运算符以及析构函数还有移动构造函数 移动赋值操作符 ，可以重构或者禁用他们。
//同样定义了诸如 new，delete这样的运算符，运算符可以重载这些函数
//c++11 之后，允许我们显式的声明采用或拒绝编译器自带的函数。 默认采用用default，拒绝用delete

//这里有个350原则。也就是{析构函数，拷贝构造函数，拷贝赋值操作符} 三对齐
//{析构函数，拷贝构造函数，拷贝赋值操作符，移动构造函数，移动赋值操作符 } 五对齐
//0的含义是，五对齐中一个都不实现，方便c++的资源管理类，比如智能指针。
class Magic{
public:
	Magic(){};//默认构造函数
	Magic& operator=( const Magic&)=delete;//赋值运算符
	Magic(int v){
		value2 = v;
	}
	//使用默认的拷贝构造函数
	Magic(const Magic&)=default;
	~Magic() = default;
	// 重载 new 运算符 
    void* operator new(size_t size) 
    {     
        std::cout << "Custom new for size " << size << std::endl; 
        void* storage = std::malloc(size); 
        if (nullptr == storage) { throw std::bad_alloc(); } 
        return storage; 
    } 
    // 重载 delete 运算符 
    void operator delete(void* p) { 
        std::cout << "Custom delete" << std::endl; 
        std::free(p); 
    } 
    // 如果需要，还可以重载 new[] 和 delete[] 
    void* operator new[](size_t size) { 
        // 类似于 operator new，但用于数组 
        std::cout << "Custom new[] for size " << size << std::endl; 
        void* storage = std::malloc(size); 
        if (nullptr == storage) { throw std::bad_alloc(); } 
        return storage; 
    } 
    void operator delete[](void* p) { 
        // 类似于 operator delete，但用于数组 
        std::cout << "Custom delete[] " << std::endl; 
        std::free(p); 
    } 
    
    // Placement new操作符重载
    void* operator new(std::size_t size, void* ptr) noexcept {
        return ptr;
    }
    
	int value1{0};
	int value2;
};
int main(){
	Magic* p = new Magic(20);
	cout<<"Magic : "<<p->value1<<" , " <<p->value2<<endl;
	delete p;
	Magic* parray = new Magic[10];//注意默认构造函数不能拒绝。
	for(int i=0; i<10; i++)
		cout<<"Magic : "<<parray[i].value1<<" , " <<parray[i].value2<<endl;
	delete[] parray;
	
	char buffer[sizeof(Magic)*3];
	Magic* arrayPtr = reinterpret_cast<Magic*>(buffer);
	for(int i=0;i<3;i++) new (arrayPtr+i) Magic(i);// 使用placement new
	for(int i=0;i<3;i++) arrayPtr[i].~Magic();
}