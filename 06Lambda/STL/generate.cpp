#include <algorithm>
#include <iostream>
#include <vector>
#include <numeric>
#include <random>

template<typename T> 
void println(std::string_view fmt, T const& v)
{
	std::cout << fmt;
	auto it = std::begin(v);
	auto end = std::end(v);	
    if( it!=end ){
		std::cout<<*it++;
		while(it!=end){
			std::cout<<","<<*it++;
		}
    }
    std::cout << '\n';
};
 
 
int f()
{ 
    static int i;
    return ++i;
}
template<int min, int max>
int rand_int()
{
    static std::uniform_int_distribution dist(min, max);
    static std::mt19937 gen(std::random_device{}());
    return dist(gen);
}
 
int main()
{
    std::vector<int> v(5);
 
    std::generate(v.begin(), v.end(), f);
    println("v: ", v);
 
    // 以来自 lambda 函数的默认值 0,1,2,3,4 初始化
    // 等价于 std::iota(v.begin(), v.end(), 0);
    std::generate(v.begin(), v.end(), [n = 0] () mutable { return n++; });
    println("v: ", v);
    
    /*std::iota 是定义在 <numeric> 头文件中的一个函数模板，它用于给一个序列赋值，使得序列中每个元素比前一个元素大1。第三个参数指定了序列中的第一个元素的值。
    */
    std::iota(v.begin(), v.end(), 0);//iota 可以是基础的序列。
    println("v: ", v);
    
    //使用transform *2
    std::transform(v.begin(),v.end(),v.begin(),[](int v){return v*2;});
    println("v: ", v);
    
    std::generate_n(std::back_inserter(v), 20, rand_int<-9, +9>);
    println("v: ", v);
}