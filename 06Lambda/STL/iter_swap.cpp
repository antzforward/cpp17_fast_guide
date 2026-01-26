#include <algorithm>
#include <iostream>
#include <random>
#include <string_view>
#include <vector>
 
template<class ForwardIt>
void selection_sort(ForwardIt begin, ForwardIt end)
{
    for (ForwardIt it = begin; it != end; ++it)
        std::iter_swap(it, std::min_element(it, end));//返回范围中的最小元素，然后进行交换。
}
 
void println(std::string_view rem, std::vector<int> const& v)
{
    std::cout << rem;
    for (int e : v)
        std::cout << e << ' ';
    std::cout << '\n';
}

/*每次你调用 rand_int<min_value, max_value>()，它都会返回一个在 min_value 和 max_value 之间（包括这两个值）的随机整数。由于 dist 和 gen 被声明为静态局部变量，它们只被初始化一次，即第一次调用该特定实例化的函数时。这意味着对于给定的模板参数，随机数发生器将保持状态，并且每次调用 rand_int 都会基于上一次产生的随机数来生成下一个随机数。*/ 
template<int min, int max>
int rand_int()
{
    static std::uniform_int_distribution dist(min, max);
    static std::mt19937 gen(std::random_device{}());
    return dist(gen);
}
 
int main()
{
    std::vector<int> v;
    std::generate_n(std::back_inserter(v), 20, rand_int<-9, +9>);
 
    std::cout << std::showpos;
    println("排序前: ", v);
    selection_sort(v.begin(), v.end());
    println("排序后: ", v);
}