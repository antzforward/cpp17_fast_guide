#include <algorithm>
#include <functional>
#include <iostream>
#include <vector>
 
int main()
{
    std::vector<int> v1{0, 1, 2, 3, 40, 40, 41, 41, 5};
 
    auto i1 = std::adjacent_find(v1.begin(), v1.end());
 
    if (i1 == v1.end())
        std::cout << "没有匹配的相邻元素\n";
    else
        std::cout << "第一对相等的相邻元素位于 "
                  << std::distance(v1.begin(), i1) << "，*i1 = "
                  << *i1 << '\n';
 
    auto i2 = std::adjacent_find(v1.begin(), v1.end(), std::greater<int>());
    if (i2 == v1.end())
        std::cout << "整个 vector 已经是升序的\n";
    else
        std::cout << "非降序子序列中最后的元素位于 "
                  << std::distance(v1.begin(), i2) << "，*i2 = " << *i2 << '\n';
}