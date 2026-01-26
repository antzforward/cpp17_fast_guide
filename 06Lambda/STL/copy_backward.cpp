#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>

/*将范围 [first, last) 内的元素复制到终于 d_last 的范围。以逆序复制元素（首先复制末元素），但保持相对顺序。
如果 d_last 在 (first, last) 中，那么行为未定义。此时必须用 std::copy 取代 std::copy_backward。*/ 
int main()
{
    std::vector<int> source(4);
    std::iota(source.begin(), source.end(), 1); // 填充 1, 2, 3, 4
 
    std::vector<int> destination(6);
 
    std::copy_backward(source.begin(), source.end(), destination.end());
 
    std::cout << "destination 包含：";
    for (auto i: destination)
        std::cout << i << ' ';
    std::cout << '\n';
}