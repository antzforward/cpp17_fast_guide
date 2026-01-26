#include <algorithm>
#include <cassert>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <string_view>
#include <vector>
 
using namespace std::literals;
 
template<typename Container>
bool contains(const Container& cont, std::string_view s) {
    return std::search(cont.begin(), cont.end(), s.begin(), s.end()) != cont.end();
}
 
int main()
{
    const auto str{"why waste time learning, when ignorance is instantaneous?"sv};
    assert(contains(str, "learning"));
    assert(!contains(str, "lemming"));
 
    const std::vector vec(str.begin(), str.end());
    assert(contains(vec, "learning"));
    assert(!contains(vec, "leaning"));
 
    // 演示 C++17 使用搜索器的重载：
    constexpr auto quote
    {
        "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed "
        "do eiusmod tempor incididunt ut labore et dolore magna aliqua"sv
    };
 
    for (const auto word : {"pisci"sv, "Pisci"sv})
    {
        std::cout << "字符串 " << std::quoted(word) << ' ';
        const std::boyer_moore_searcher searcher(word.begin(), word.end());
        const auto it = std::search(quote.begin(), quote.end(), searcher);
        if (it == quote.end())
            std::cout << "未找到\n";
        else
            std::cout << "在偏移 " << it - quote.begin() << " 处找到\n";
    }
}