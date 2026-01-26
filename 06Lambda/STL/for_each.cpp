#include <algorithm>
#include <iostream>
#include <vector>
 
int main()
{
    std::vector<int> v {3, -4, 2, -8, 15, 267};
 
    auto print = [](const int& n) { std::cout << n << ' '; };
 
    std::cout << "before:";
    std::for_each(v.cbegin(), v.cend(), print);
    std::cout << '\n';
 
    // 就地自增每个元素
    std::for_each(v.begin(), v.end(), [](int &n){ n++; });
 
    std::cout << "after:";
    std::for_each(v.cbegin(), v.cend(), print);
    std::cout << '\n';
 
    struct Sum
    {
        void operator()(int n) { sum += n; }
        int sum {0};
    };
 
    // 对每个元素调用 Sum::operator()
    Sum s = std::for_each(v.begin(), v.end(), Sum());
    std::cout << "sum:" << s.sum << '\n';
}
