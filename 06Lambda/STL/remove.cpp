#include <algorithm>
#include <cctype>
#include <iomanip>
#include <iostream>
#include <string>
#include <iterator>
#include <string_view>
 
int main()
{
    std::string str1 = "Text with some   spaces";
 
    auto noSpaceEnd = std::remove(str1.begin(), str1.end(), ' ');
 
    // 空格只是逻辑上从字符串被移除。
    // 通过视图可以发现原始字符串并没有缩小：
   // 修正：使用字符串开始地址和新长度创建 string_view
    std::cout << std::string_view(&*str1.begin(), std::distance(str1.begin(), noSpaceEnd))
              << " 大小：" << str1.size() << '\n';
 
    str1.erase(noSpaceEnd, str1.end());
 
    // 物理移除字符串中的空格。
    std::cout << str1 << " 大小：" << str1.size() << '\n';
 
    std::string str2 = "Text\n with\tsome \t  whitespaces\n\n";
    str2.erase(std::remove_if(str2.begin(), 
                              str2.end(),
                              [](unsigned char x){ return std::isspace(x); }),
               str2.end());
    std::cout << str2 << '\n';
    
    //from remove_copy
    std::string str = "#返回#值#优化";
    std::cout << "擦除前：" << std::quoted(str) <<" 大小 "<<str.size()<< "\n";
 
    std::cout << "擦除后：\"";
    //对比可以看出 第三个参数为可以为转换操作，可以把remove 和 cout结合起来了。
    std::remove_copy(str.begin(), str.end(),
                     std::ostream_iterator<char>(std::cout), '#');
    std::cout <<" 大小 "<<str.size()<< "\"\n";
}