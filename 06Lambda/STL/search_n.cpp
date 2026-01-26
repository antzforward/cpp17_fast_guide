#include <algorithm>
#include <iostream>
#include <iterator>
#include <cassert>
 
template<class Container, class Size, class T>
[[nodiscard]]
constexpr bool consecutive_values(const Container& c, Size count, const T& v)
{
    return std::search_n(std::begin(c), std::end(c), count, v) != std::end(c);
}
 
int main()
{
    constexpr char sequence[] = "1001010100010101001010101";
 
    assert(consecutive_values(sequence, 3, '0'));
 
    std::cout << std::boolalpha
              << "有 4 个连续的零："
              << consecutive_values(sequence, 4, '0') << '\n'
              << "有 3 个连续的零："
              << consecutive_values(sequence, 3, '0') << '\n';
}