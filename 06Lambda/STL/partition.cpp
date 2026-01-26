#include <algorithm>
#include <array>
#include <forward_list>
#include <iostream>
#include <iterator>
#include <vector>
#include <numeric>

/*partition 重排序范围 [first, last) 中的元素，使得谓词 p 对其返回 true 的元素位于谓词 p 对其返回 false 的元素之前。不保持相对顺序。*/ 

auto print = [](auto const remark, auto const& v)
{
    std::cout << remark;
    for (auto n : v)
        std::cout << n << ' ';
    std::cout << '\n';
};

//分块的递归，这个方法弄到rust里面怎么写？
template <class ForwardIt>
void quicksort(ForwardIt first, ForwardIt last)
{
    if (first == last)//递归停止，首要。
        return;
	//分对称的一半。
    auto pivot = *std::next(first, std::distance(first, last) / 2);
    //找到左右对称的点
    auto middle1 = std::partition(first, last, [pivot](const auto& em)
    {
        return em < pivot;
    });
    auto middle2 = std::partition(middle1, last, [pivot](const auto& em)
    {
        return !(pivot < em);
    });
 
    quicksort(first, middle1);
    quicksort(middle2, last);
}

int main()
{
    std::array<int, 9> v {1, 2, 3, 4, 5, 6, 7, 8, 9};
 
    auto is_even = [](int i) { return i % 2 == 0; };
    std::cout.setf(std::ios_base::boolalpha);
    std::cout << std::is_partitioned(v.begin(), v.end(), is_even) << ' ';
 
    std::partition(v.begin(), v.end(), is_even);
    std::cout << std::is_partitioned(v.begin(), v.end(), is_even) << '\n';
	print("partition 之后： ", v);
	
    std::reverse(v.begin(), v.end());
    std::cout << std::is_partitioned(v.cbegin(), v.cend(), is_even) << ' ';
    std::cout << std::is_partitioned(v.crbegin(), v.crend(), is_even) << '\n';
    
    std::iota(v.begin(),v.end(),1);
    print("原来的 vector：", v);
    auto it = std::partition(v.begin(), v.end(), [](int i) {return i % 2 == 0;});
 
    std::cout << "\n划分后的 vector：\n    ";
    std::copy(std::begin(v), it, std::ostream_iterator<int>(std::cout, " "));
    std::cout << "* ";
    std::copy(it, std::end(v), std::ostream_iterator<int>(std::cout, " "));
    
    std::forward_list<int> fl = {1, 30, -4, 3, 5, -4, 1, 6, -8, -4, 2, -5, 64, 1, 92};
    std::cout << "\n未排序的列表：\n    ";
    for (int n : fl)
        std::cout << n << ' ';
 
    quicksort(std::begin(fl), std::end(fl));
    std::cout << "\n用 quicksort 排序后：\n    ";
    for (int fi : fl)
        std::cout << fi << ' ';
    std::cout << '\n';
    
    std::vector<int> v2 {0, 0, 3, -1, 2, 4, 5, 0, 7};
    print("初始vector ： ", v2);
    std::stable_partition(v2.begin(), v2.end(), [](int n) { return n > 0; });
    print("使用stable_partition:",v2);
    
    std::vector<int> v3 {0, 0, 3, -1, 2, 4, 5, 0, 7};
    std::partition(v3.begin(), v3.end(), [](int n) { return n > 0; });
    print("使用 partition:",v3);
    
    //partition_copy 跟所有带copy的情况
    int arr[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int true_arr[5] = {0};
    int false_arr[5] = {0};
 
    std::partition_copy(std::begin(arr), std::end(arr),
                        std::begin(true_arr), std::begin(false_arr),
                        [](int i) { return 4 < i; });
 
    print("true_arr:  ", true_arr);
    print("false_arr: ", false_arr);
}