#include <algorithm>
#include <iostream>
#include <iterator>
#include <numeric>
#include <vector>

/*复制范围 [first, last) 中的元素到从 d_first 开始的另一范围（复制目标范围）。

1) 按从 first 到 last 的顺序复制 [first, last) 中的所有元素。
 如果 d_first 在 [first, last) 中，那么行为未定义。此时可以用 std::copy_backward 代替。*/ 
int main()
{
    std::vector<int> from_vector(10);
    std::iota(from_vector.begin(), from_vector.end(), 0);
 
    std::vector<int> to_vector;
    std::copy(from_vector.begin(), from_vector.end(),
              std::back_inserter(to_vector));
// 或者可以使用另一种方式，
//  std::vector<int> to_vector(from_vector.size());
//  std::copy(from_vector.begin(), from_vector.end(), to_vector.begin());
// 任一方式都等价于
//  std::vector<int> to_vector = from_vector;
 
    std::cout << "to_vector 包含：";
 
    std::copy(to_vector.begin(), to_vector.end(),
              std::ostream_iterator<int>(std::cout, " "));
    std::cout << '\n';
 
    std::cout << "to_vector 中的奇数有：";
 
    std::copy_if(to_vector.begin(), to_vector.end(),
                 std::ostream_iterator<int>(std::cout, " "),
                 [](int x) { return x % 2 != 0; });
    std::cout << '\n';
 
    std::cout << "to_vector 中 3 的倍数有：";
 
    to_vector.clear();
    std::copy_if(from_vector.begin(), from_vector.end(),
                 std::back_inserter(to_vector),
                 [](int x) { return x % 3 == 0; });
 
    for (int x : to_vector)
        std::cout << x << ' ';
    std::cout << '\n';
}