#include <algorithm>
#include <iostream>
#include <vector>
#include <numeric>
 
auto print = [](auto const remark, auto const& v)
{
    std::cout << remark;
    for (auto n : v)
        std::cout << n << ' ';
    std::cout << '\n';
};
/*进行元素范围上的左旋转。
 具体而言，std::rotate 交换范围 [first, last) 中的元素，将 [first, middle) 中的元素放在 [middle, last) 后面并且保留这两个范围中元素的原本顺序。
要右旋转，常规的是用reverse iterator来实现。 
*/ 
int main()
{
    std::vector<int> v{2, 4, 2, 0, 5, 10, 7, 3, 7, 1};
    print("排序前：\t\t", v);
 
    // 插入排序
    for (auto i = v.begin(); i != v.end(); ++i)
        std::rotate(std::upper_bound(v.begin(), i, *i), i, i + 1);
    print("排序后：\t\t", v);
 
    // 简单向左旋转
    std::rotate(v.begin(), v.begin() + 1, v.end());
    print("简单向左旋转：\t", v);
 
    // 简单向右旋转
    std::rotate(v.rbegin(), v.rbegin() + 1, v.rend());
    print("简单向右旋转：\t", v);
    
    std::vector<int> from_vector(10);
    std::iota(from_vector.begin(), from_vector.end(), 0);
    print("简单向左旋转之前：\t", from_vector);
    std::vector<int> dest;
    std::rotate_copy(from_vector.begin(), 
    from_vector.begin() + std::distance(from_vector.begin(),from_vector.end())/2, 
    from_vector.end(), 
    std::back_inserter(dest));
    print("dest  简单向左旋转：\t", dest);
    print("source简单向左旋转：\t", from_vector);
    
     // copy the rotation result directly to the std::cout
    auto pivot = std::find(dest.begin(), dest.end(), 1);
    std::rotate_copy(dest.begin(), pivot, dest.end(),
                     std::ostream_iterator<int>(std::cout, " "));
    std::cout << '\n';
    
}