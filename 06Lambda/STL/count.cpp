#include <algorithm>
#include <array>
#include <iostream>
#include <iterator>
#include <cassert>
 
int main()
{
    constexpr std::array v = {1, 2, 3, 4, 4, 3, 7, 8, 9, 10};
    std::cout << "v：";
    std::copy(v.cbegin(), v.cend(), std::ostream_iterator<int>(std::cout, " "));
    std::cout << '\n';
 
    // 确定有多少个整数匹配目标值。
    for (const int target: {3, 4, 5})
    {
        const int num_items = std::count(v.cbegin(), v.cend(), target);
        std::cout << "目标数字：" << target << "，计数：" << num_items << '\n';
    }
 
    // 用 lambda 表达式计量能被 4 整除的元素数。
    int count_div4 = std::count_if(v.begin(), v.end(), [](int i) { return i % 4 == 0; });
    std::cout << "能被 4 整除的数字个数：" << count_div4 << '\n';
 
    // 简化版的 distance，具有 O(N) 复杂度：
    auto distance = [](auto first, auto last)
    {
        return std::count_if(first, last, [](auto){ return true; });
    };
    assert(distance(v.begin(), v.end()) == 10);
}