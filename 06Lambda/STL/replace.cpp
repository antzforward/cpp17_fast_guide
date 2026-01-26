#include <algorithm>
#include <array>
#include <functional>
#include <iostream>
/*以 new_value 替换范围 [first, last) 中所有满足特定判别标准的元素。
1) 替换所有等于（用 operator== 比较）old_value 的元素。
3) 替换所有谓词 p 对其返回 true 的元素。*/ 
int main()
{
    std::array<int, 10> s{5, 7, 4, 2, 8, 6, 1, 9, 0, 3};
	for (int a : s)
        std::cout << a << ' ';
    std::cout << '\n';
    
    std::replace(s.begin(), s.end(), 8, 88);
 
    for (int a : s)
        std::cout << a << ' ';
    std::cout << '\n';
 
    std::replace_if(s.begin(), s.end(), 
                    [](int value) { return value < 5; }, 55);//bind的用法，这里要用lambda
 
    for (int a : s)
        std::cout << a << ' ';
    std::cout << '\n';
}