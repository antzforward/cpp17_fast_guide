#include <algorithm>
#include <iostream>
#include <vector>
 
void println(const std::vector<int>& v) {
    auto count = v.size(); // 显式声明count的类型并初始化
    for (const auto& e : v)
        std::cout << e << (--count ? ", " : "\n");
}
 
int main()
{
    std::vector<int> vi {1, 2, 3, 4, 5};
    println(vi);
 
    std::for_each_n(vi.begin(), 3, [](auto& n) { n *= 2; });
    println(vi);
}