#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
 
auto print_result = [](auto result, const auto& v)
{
    result == v.end()
        ? std::cout << "未找到序列\n"
        : std::cout<<*result << " 最后一次在位置 " << std::distance(v.begin(), result)
                    << " 出现\n";
};
 
int main()
{
    const auto v = {1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4};
 
    for (auto const& x : {std::array{1, 2, 3}, {4, 5, 6}})
    {
        auto iter = std::find_end(v.begin(), v.end(), x.begin(), x.end()); // 重载 (1)
        print_result(iter, v);
    }
 
    for (auto const& x : {std::array{-1, -2, -3}, {-4, -5, -6}})
    {
        auto iter = std::find_end(v.begin(), v.end(), x.begin(), x.end(), // 重载 (3)
                                  [](int x, int y)
                                  {
                                      return std::abs(x) == std::abs(y);
                                  });
        print_result(iter, v);
    }
}