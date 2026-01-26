#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

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
    std::vector<int> v {1, 2, 3};
    println("反转之前, v = ", v);
    std::reverse(v.begin(), v.end());
    println("反转之后, v = ", v);
 
    int a[] = {4, 5, 6, 7};
    println("反转之前, a = ", a);
    std::reverse(std::begin(a), std::end(a));
    println("反转之后, a = ", a);
    
    //所有带copy的都有可能，
    std::vector<int> destination;//destination 是空的，要么扩容量，
    //要么用一种方法是使用std::back_inserter来自动增长destination大小
    // 错误修正：确保有足够空间存储反转的元素
    std::reverse_copy(std::begin(a), std::end(a), std::back_inserter(destination));
    println("反转到新的vector，destination：",destination);
 
	std::cout << "反转之后：\"";
	// 注意：这里不会修改v，仅仅将反转后的结果输出到stdout
    std::reverse_copy(std::begin(v), std::end(v), std::ostream_iterator<int>(std::cout," "));
    std::cout<<"\"\n";
    
    println("反转之后的之后, v = ", v);
}