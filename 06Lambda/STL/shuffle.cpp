#include <algorithm>
#include <iostream>
#include <iterator>
#include <random>
#include <vector>
#include <numeric>

template<typename T,typename Container > 
void println(T rem, Container const& v)
{
	std::cout << rem<<'\t'; 
    for ( auto e : v)
        std::cout << e << ' ';
    std::cout << '\n';
}

//random_shuffle 被shuffle替换了，shuffle可以指用随机器，这样就可以生成所有的随机数组了。
int main()
{
    std::vector<int> v{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
 
    std::random_device rd;
    std::mt19937 g(rd());
 
    std::shuffle(v.begin(), v.end(), g);
 
    std::copy(v.begin(), v.end(), std::ostream_iterator<int>(std::cout, " "));
    std::cout << '\n';
    
    //写一个0-9的全随机过程
    std::vector<int> numbers(3);
    std::iota(numbers.begin(), numbers.end(), 0);
    int count = 0;
    do println(count++,numbers);
    while (std::next_permutation(numbers.begin(), numbers.end()));
}