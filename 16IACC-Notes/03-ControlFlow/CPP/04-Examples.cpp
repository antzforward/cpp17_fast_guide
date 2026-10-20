// cl /EHsc /std:c++20 06-Sort3Nums.cpp
#include <algorithm>
#include <format>
#include <array> 
#include <iostream>
using namespace std;
//第二章的习题挨个做
int main(int argc, char* argv[]) {
    int problem = 0;
    int branch = 0;
    if (argc >= 3) {
        problem = std::stoi(argv[1]);
        branch = std::stoi(argv[2]);
    }
    if( problem == 0 || branch == 0){
        cout<<format(R"(
        问题列表
            1、水仙花数
            2、韩信点兵
            3、子序列的和
            4、分数化小数
            5、排列
        不同写法：
            1、暴力解法
            2、STL的API使用
            3、算法最优的写法
        )");
        return 0;
    }
    return 0;
}