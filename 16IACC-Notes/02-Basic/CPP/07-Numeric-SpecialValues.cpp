// cl /EHsc /std:c++20 07-Numeric-SpecialValues.cpp
#include <iostream>
#include <limits>
#include <type_traits>
#include <string>

template <typename T>
void show(const std::string& name) {
    using L = std::numeric_limits<T>;

    std::cout << "===== " << name << " =====\n";
    std::cout << "sizeof = " << sizeof(T) << " bytes\n";
    std::cout << std::boolalpha;
    std::cout << "is_signed = " << L::is_signed << '\n';
    std::cout << "is_integer = " << L::is_integer << '\n';
    std::cout << "is_exact = " << L::is_exact << '\n';
    std::cout << "digits = " << L::digits << '\n';
    std::cout << "digits10 = " << L::digits10 << '\n';
    std::cout << "max_digits10 = " << L::max_digits10 << '\n';

    if constexpr (std::is_integral_v<T>) {
        // 对 char/bool 等做一元 +，输出整数形式
        std::cout << "min = " << +L::min() << '\n';
        std::cout << "max = " << +L::max() << '\n';
        std::cout << "lowest = " << +L::lowest() << '\n';
    } else {
        // 浮点类型注意：min() 是最小正正规数，不是最小负数
        std::cout << "min (最小正正规数) = " << L::min() << '\n';
        std::cout << "max = " << L::max() << '\n';
        std::cout << "lowest (最小负数) = " << L::lowest() << '\n';
        std::cout << "epsilon = " << L::epsilon() << '\n';
        std::cout << "denorm_min = " << L::denorm_min() << '\n';

        if (L::has_infinity)
            std::cout << "infinity = " << L::infinity() << '\n';
        if (L::has_quiet_NaN)
            std::cout << "quiet_NaN = " << L::quiet_NaN() << '\n';
    }

    T zero{};
    std::cout << "零初始化 T{} = " << +zero << '\n';
    std::cout << '\n';
}

int main() {
    show<bool>("bool");
    show<char>("char");
    show<signed char>("signed char");
    show<unsigned char>("unsigned char");
    show<short>("short");
    show<unsigned short>("unsigned short");
    show<int>("int");
    show<unsigned int>("unsigned int");
    show<long>("long");
    show<unsigned long>("unsigned long");
    show<long long>("long long");
    show<unsigned long long>("unsigned long long");

    show<float>("float");
    show<double>("double");
    show<long double>("long double");
    return 0;
}