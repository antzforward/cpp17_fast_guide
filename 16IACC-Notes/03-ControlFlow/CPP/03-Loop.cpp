// cl /EHsc /std:c++23preview /permissive- 03-Loop.cpp 
#include <algorithm>
#include <format>
#include <array> 
#include <string>
#include <iostream>
using namespace std;
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
            1、aabb的完全平方数
            2、韩信点兵
            3、求pi/4的近似值
            4、求 e的近似值
            5、阶乘之和
            6、欧几里得除法的形式
        不同写法：
            1、暴力解法
            2、STL的API使用
            3、算法最优的写法
        )");
        return 0;
    }
    if( problem == 1 ){
        //1、找到表现形式为aabb的完全平方数
        if(branch == 1 ){
          //暴力法：就是遍历形式      
          for(int a=1;a<=9;a++){
            for(int b=0;b<=9;b++){
                int n = a*1100+b*11;
                int m = std::sqrt(n);
                if( m * m == n ){
                    cout<<format("{},",n);
                }
            }
          }
          cout<<format("\n");
          return 0;
        }else if( branch == 2 ){
            //用foreach的形式 性能差不多就是为了好看
        }else if(branch > 2 ){
            //真正的优化形式 考虑最快
        }
    }else if( problem == 2){
        // n_0 = a, n_{i+1} = n_i % 2?3*n_i+1:int(n_i>>1)
        // if n_i == 1 ? i : n_i = n_{i+1} = n_i % 2?3*n_i+1:int(n_i>>1)
        //因为这个是顺推式的，就不用递归调用了，写while do形式吧
        //为了学习 for语句，就对比写个结构完成的for，for等同于initial，check，update
    } else if(problem == 3){
        // 求\dfrac{\pi}4的近似值
        // 不考虑那么多精度，直接反函数求解 \dfrac{\arccos(-1)}{4} 的形式
        // 用泰勒展示来求？
    }else if(problem == 4){
        //求 e的近似值，
        // 也有用反函数形式求的？
        // 用展式来求。
    }else if(problem == 5){
        //阶乘之和，要考虑accumulate的值超过范围的问题。
    }else if(problem == 6 ){
        //欧几里得除法的形式
    }
    
}