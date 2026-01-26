#include <algorithm>
#include <iostream>
#include <vector>
#include <iterator>
#include <string>
//注意unque的算法不是清理所有的重复，而是只清理连续的重复内容，这个部分一定要仔细看结果。 
//Python的自带unique应该不同。先sort，再unique 才能起到unique的效果，这也符合集合不排序的特点。
//与之关联的有adjacent_find 查找首对相邻的相同元素。
int main()
{
    // 含有数个重复元素的 vector
    std::vector<int> v{1, 2, 1, 1, 3, 3, 3, 4, 5, 4};
    auto i1 = std::adjacent_find(v.begin(), v.end());
 
    if (i1 == v.end())
        std::cout << "没有匹配的相邻元素\n";
    else
        std::cout << "第一对相等的相邻元素位于 "
                  << std::distance(v.begin(), i1) << "，*i1 = "
                  << *i1 << '\n';
                  
    auto print = [&] (int id)
    {
        std::cout << "@" << id << ": ";
        for (int i : v)
            std::cout << i << ' ';
        std::cout << '\n';
    };
    print(1);
 
    // 移除相继（毗邻）的重复元素
    auto last = std::unique(v.begin(), v.end());
    // v 现在保有 {1 2 1 3 4 5 4 x x x}，其中 x 不确定
    v.erase(last, v.end());
    print(2);
 
    // sort 后 unique 以移除所有重复
    std::sort(v.begin(), v.end()); // {1 1 2 3 4 4 5}
    print(3);
 
    last = std::unique(v.begin(), v.end());
    // v 现在保有 {1 2 3 4 5 x x}，其中 x 不确定
    v.erase(last, v.end());
    print(4);
    
    //unique copy,其中copy主要的用法是导出一个输出cout过程。
    std::string s1 {"The      string    with many       spaces!"};
    std::cout << "去重前：" << s1 << '\n';
 
    std::string s2;
    std::unique_copy(s1.begin(), s1.end(), std::back_inserter(s2),
                     [](char c1, char c2){ return c1 == ' ' && c2 == ' '; });
 
    std::cout << "去重后：" << s2 << '\n';
    std::cout << "去重后 旧的string 并没有变化：" << s1 << '\n';
}