// cl /EHsc /std:c++20 01-helloworld.cpp
#include <cmath>
#include <format>
#include <iostream>

int main() {
    std::cout << std::format("{}\n", 1 + 2);
    std::cout << std::format("{:.1f}\n", 8.0 / 5.0);
    std::cout << std::format("{:.8f}\n", 1 + 2 * std::sqrt(3) / (5 - 0.1));
    int a,b;
    std::cin>>a>>b;
    std::cout << std::format("{}\n", a + b);
    return 0;
}
