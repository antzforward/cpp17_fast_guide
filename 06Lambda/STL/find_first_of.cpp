#include <algorithm>
#include <iostream>
#include <vector>
//find first of 这是求并集吗？ 
auto print_sequence = [](auto const id, auto const& seq, int pos = -1)
{
	int i=0;
    std::cout << id << "{ ";
    for ( auto const& e : seq)
    {
        const bool mark{i == pos};
        std::cout << (i++ ? ", " : "");
        std::cout << (mark ? "[ " : "") << e << (mark ? " ]" : "");
    }
    std::cout << " }\n";
};
 
int main()
{
    const std::vector<int> v{0, 2, 3, 25, 5};
    const auto t1 = {19, 10, 3, 4};
    const auto t2 = {1, 6, 7, 9};
 
    auto find_any_of = [](const auto& v, const auto& t)
    {
        const auto result = std::find_first_of(v.begin(), v.end(),
                                               t.begin(), t.end());
        if (result == v.end())
        {
            std::cout << "v 和 t 中没有相等的元素\n";
            print_sequence("t = ", t);
            print_sequence("v = ", v);
        }
        else
        {
            const auto pos = std::distance(v.begin(), result);
            std::cout << "在位置 " << pos << " 找到匹配 (" << *result << ")\n";
            print_sequence(", where t = ", t);
            print_sequence("v = ", v, pos);
        }
    };
 
    find_any_of(v, t1);
    find_any_of(v, t2);
}