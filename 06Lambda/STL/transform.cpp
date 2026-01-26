#include <algorithm>
#include <cctype>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
 
void print_ordinals(std::vector<std::size_t> const& ordinals)
{
    std::cout << "序数：";
    for (std::size_t ord : ordinals)
        std::cout << std::setw(3) << ord << ' ';
    std::cout << '\n';
}
 
int main()
{
    std::string s{"hello"};
    std::transform(s.cbegin(), s.cend(),
                   s.begin(), // 写入相同位置
                   [](unsigned char c) { return std::toupper(c); });
    std::cout << "s = " << std::quoted(s) << '\n';
 
    // 使用 std::for_each 实现相同效果（见上文注解）
    std::string g{"hello"};
    std::for_each(g.begin(), g.end(), [](char& c) // 当场修改
    {
        c = std::toupper(static_cast<unsigned char>(c));
    });
    std::cout << "g = " << std::quoted(g) << '\n';
 
    std::vector<std::size_t> ordinals;
    //std::back_inserter(ordinals) 可以认为end的前一个iter 可以方便使用。用distance来表示也行吧。
    std::transform(s.cbegin(), s.cend(), std::back_inserter(ordinals),
                   [](unsigned char c) { return c; });
 
    print_ordinals(ordinals);
	/*std::transform 应用给定的函数到某个范围并将结果存储到始于 d_first 的另一范围。
	1) 应用一元函数 unary_op 到 [first1, last1) 所定义的范围。
	3) 应用二元函数 binary_op 到来自两个范围的元素对：一个以 [first1, last1) 定义，而另一个始于 first2。*/
	//理论上可以放在3个不同的container里面。
    std::transform(ordinals.cbegin(), ordinals.cend(), ordinals.cbegin(),
                   ordinals.begin(), std::plus<>{});//plus执行加法，定义struct，默认做x+y，从结果看都是自身。
 
    print_ordinals(ordinals);
}