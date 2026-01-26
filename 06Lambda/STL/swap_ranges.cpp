#include <algorithm>
#include <iostream>
#include <list>
#include <vector>
 
auto print = [](auto comment, auto const& seq)
{
    std::cout << comment;
    for (const auto& e : seq)
        std::cout << e << ' ';
    std::cout << '\n';
};
 
int main()
{
    std::vector<char> v{'a', 'b', 'c', 'd', 'e'};
    std::list<char> l{'1', '2', '3', '4', '5'};
 
    print("swap_ranges 之前:\n" "v: ", v);
    print("l: ", l);
 
    std::swap_ranges(v.begin(), v.begin() + 3, l.begin());
 
    print("swap_ranges 之后:\n" "v: ", v);
    print("l: ", l);
}