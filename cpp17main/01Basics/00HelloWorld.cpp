#include <iostream>
#include <vector>
#include "../TestMacro.h"
/**
 * @file 00HelloWorld.cpp
 * @ingroup 01Basics
 * @brief 性能测试入门 — PERF_START/PERF_END 宏的使用示例
 * @details 演示如何使用自定义的 PERF 宏对内存分配进行性能基准测试，
 *          包括一维 vector 和二维 vector 的分配场景。
 *          编译时建议开启 O2 优化以获得有意义的性能数据。
 */
// 使用示例
int main() {
	// added the two lines below
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(NULL);  
    PERF_START(test1)
    
    /** 测试代码 - 分配一些内存*/
    std::vector<int> vec;
    for (int i = 0; i < 1000000; i++) {
        vec.push_back(i);
    }
    
    PERF_END(test1)
    
    PERF_START(test2)
    
    /** 更多内存分配 */
    std::vector<std::vector<int>> matrix;
    for (int i = 0; i < 1000; i++) {
        matrix.emplace_back(1000, i);
    }
    
    PERF_END(test2)
    
    return 0;
}
//cl /EHsc /std:c++17 00HelloWorld.cpp /link psapi.lib 