#include <iostream>
#include <functional>
#include <vector>

using foo=void(int);//替代的是typedef void(*foo)(int);更广泛一点，它是类型

void functional(foo f){ //函数指针作为函数参数
	f(1);
}
//因为function没有比较操作，因此不能做-=操作啊。
class Multicast {
public:
    void operator+=(const std::function<void()>& func) {
        functions.push_back(func);
    }

    void operator-=(const std::function<void()>& func) {
        // 这里需要一些额外的逻辑来移除特定的函数
        // 由于 function 对象没有比较相等的操作，这可能很麻烦
        // 可能需要使用其他方式来标识和移除函数
    }

    void Invoke() {
        for (auto& func : functions) {
            func();
        }
    }

private:
    std::vector<std::function<void()>> functions;
};

void hello() {
    std::cout << "Hello, ";
}

void world() {
    std::cout << "world!" << std::endl;
}

int boo(int para){
	return para * 2;
}

int main(){
	auto f=[](int value){
		std::cout<<value<<std::endl;
	};
	functional(f);
	f(1);
	
	std::function<int(int)> func = boo;
	
	int important = 10;
	std::function<int(int)> func2 = [&](int value)->int{
		return 1+value+important;
	};
	std::cout << func(10) << std::endl;
	std::cout << func2(10) << std::endl;

	Multicast multicast;

    multicast += hello;
    multicast += world;

    multicast.Invoke(); // 输出: Hello, world!
    
	return 0;
}
