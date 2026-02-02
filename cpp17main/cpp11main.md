## 为什么
还是因为要转向UE 引擎了，虽然主要的工具可能是蓝图，但是，不能只有一种方式吧。复习一下c++编程。
现在主要的c++应该是c++17，但是如果设计到Task，可能还需要更新的版本，但是c++17应该已经覆盖了很多问题的。

## 怎么做
多做练习，多进行性能比较，写一堆的单一的小代码，通过不同的性能比较，建立直接印象，在做性能方向的决策的时候，就有点用。

## 重点
还是单一的cpp，单独通过cl来编译，运行的方式，不用IDE来处理，主要的文本工具就是现在用的Notepad3
没有代码自动补全，要自己手动处理。
另外，尽量在代码注释中说明自己的代码的内容，处理结果，如果是多方案的话，给出推荐方案。
比如下面的代码
```c++
/*类型安全：std::array保证了存储在其中的元素类型是相同的，并且它的大小在编译时就已确定，这有助于避免因大小错误导致的运行时错误。
1.容器接口：std::array提供了与其他C++标准库容器类似的成员函数和操作，例如.begin(), .end(), .size(), .at(), 等等。这让std::array能够很好地和C++算法库配合工作，同时也提供了比原生数组更丰富的操作集。
2.性能：std::array与原生数组几乎有相同的性能特性，因为其数据是在栈上分配的（除非它是动态分配的对象的一部分），并且没有额外的内存开销。
3.自动管理大小：std::array知道自己的大小，这意味着你不需要单独跟踪数组的长度。
4.异常安全：由于std::array的成员函数如.at()会抛出范围之外的异常，这使得某些操作比原生数组更安全。
5.无堆分配：与std::vector不同，std::array不关心元素的增加或删除，因此它不进行堆内存分配和管理。这使得std::array在预计内存分配敏感或者禁止使用动态内存的环境中非常有用。*/
#include <algorithm>
#include <array>
#include <iostream>
#include <iterator>
#include <string>

template<typename T,typename Container > 
void println(T rem, Container const& v)
{
	std::cout << rem; 
    for ( auto e : v)
        std::cout << e << ' ';
    std::cout << '\n';
}
 
int main()
{
    // 用聚合初始化进行构造
    std::array<int, 3> a1{{1, 2, 3}}; // CWG 1270 修订前的 C++11 中要求双花括号
                                        // （C++11 之后的版本和 C++14 起不要求）
    std::array<int, 3> a2 = {1, 2, 3};  // = 后决不要求双花括号
 
    // 支持容器操作
    std::sort(a1.begin(), a1.end());
    std::reverse_copy(std::begin(a2),std::end(a2), std::ostream_iterator<int>(std::cout, " "));
    std::cout << '\n';
 
    // 支持带范围 for 循环
    std::array<std::string, 2> a3{"E", "\u018E"};
    for (const auto& s : a3)
        std::cout << s << ' ';
    std::cout << '\n';
 
    // 数组创建的推导指引 (C++17 起)
    [[maybe_unused]] std::array a4{3.0, 1.0, 4.0}; // std::array<double, 3>
    println("std::array a4{3.0, 1.0, 4.0}\t", a4);
 
    // 未指定的元素的行为与内建数组相同
    [[maybe_unused]] std::array<int, 2> a5; // 无列表初始化，a5[0] 和 a5[1] 均被默认初始化,默认不初始化。
    println("std::array<int, 2> a5\t", a5);
    [[maybe_unused]] std::array<int, 2> a6{}; // 列表初始化，两个运算均被值初始化，部分初始化，即全部初始化。
    println("std::array<int, 2> a6{}\t", a6); // a6[0] = a6[1] = 0
    [[maybe_unused]] std::array<int, 4> a7{1}; // 列表初始化，未指定的元素被值初始化，部分初始化，即全部初始化。
    println("std::array<int, 4> a7{1}\t", a7);  // a7[0] = 1, a7[1] = 0
    [[maybe_unused]] std::array<int, 8> a8; // 无列表初始化，a5[0] 和 a5[1] 均被默认初始化，全部都是随机值。
    println("std::array<int, 2> a8\t", a8);
}
```
之前把结论转入到md文件了，这次先在代码中写，然后再提纯一次吧。

讲性能的宏放在根目录下的TestMarco.h 文件中，防止反复写，不过，还是脱离了单文件编译的原则了。只能说，没办法了。
