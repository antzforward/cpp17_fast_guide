// main.cpp
#include <iostream>
#include "my_template.h"
#include "my_fake.h"

// 告诉编译器，MyTemplate<int> 的实例化在别处定义，防止在本翻译单元中重复实例化
extern template class MyTemplate<int>;

int main() {
    MyTemplate<int> instance;
    instance.doSomething();
    std::cout<<instance.msg<<'\n';
    return 0;
}