#include <iostream>
#include <vector>
#include <memory>
#include "../TestMacro.h"
// 使用示例
int main() {
    PERF_START(test1)
    
    // 测试代码 - 分配一些内存
    std::vector<int> vec;
    for (int i = 0; i < 1000000; i++) {
        vec.push_back(i);
    }
    
    PERF_END(test1)
    
    PERF_START(test2)
    
    // 更多内存分配
    std::vector<std::vector<int>> matrix;
    for (int i = 0; i < 1000; i++) {
        matrix.emplace_back(1000, i);
    }
    
    PERF_END(test2)
    
    return 0;
}
//cl /EHsc /std:c++17 00HelloWorld.cpp /link psapi.lib 