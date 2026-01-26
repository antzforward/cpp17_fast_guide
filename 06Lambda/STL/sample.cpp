#include <algorithm>
#include <iostream>
#include <iterator>
#include <random>
#include <string>
/*从序列 [first, last)（不重复地）选择 n 个元素，使得每个样本拥有相等的出现概率，并将这些被选择的元素写入到输出迭代器 out。用随机数生成器 g 生成随机数。*/ 
int main()
{
    std::string in {"ABCDEFGHIJK"}, out;
    std::sample(in.begin(), in.end(), std::back_inserter(out), 4,
                std::mt19937 {std::random_device{}()});
    std::cout << "从 " << in << " 中随机选取四个字母: " << out << '\n';
}