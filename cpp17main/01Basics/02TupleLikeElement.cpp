#include <iostream>
#include <vector>
#include <array>
#include <map>
#include <string>
//for boyer_moore_searcher
#include <algorithm>
#include <functional> //必须包含这个
#include <string_view>
#include <iomanip>
//end
#include <typeinfo>
#include <type_traits>
#include <tuple>
#include "../TestMacro.h"
// 介绍以下内容
// 1. 继承形式struct，如何unpack，如何获得属性的类型与序号
// 2. union 形式的 unpack情况
// 3. array的unpack，由于需要不支持python类似的首尾与其他的形式，这种用法其实很难用（python语法要优美很多啊）

struct B{
	int a = 1;
	int b = 2;
	operator std::tuple<int&, int&>() {
		return std::tie(a, b);
	}
};
struct D1:B{
};
struct D2:B{
	int c = 3;
	operator std::tuple<int&, int&,int&>() {
		return std::tie(a, b,c);
	}
};

#if CPP17
//增加一些内容，c++11就可以通过,c++20的时候就编译不过了，去掉了
template<typename Derived,typename Base>
constexpr bool is_base_of_v = std::is_based_of_v<Base,Derived>;
template<typename T>
void analyze_struct(){
	if constexpr (std::is_same_v<T,D2>){
		std::cout << "Type is D2\n";
		std::cout << "Has base class B\n";
		std::cout << "Has member c\n";
	}else if constexpr (std::is_same_v<T, D1>) {
		std::cout << "Type is D1\n";
		std::cout << "Has base class B\n";
	}else if constexpr (std::is_same_v<T, B>) {
		std::cout << "Type is B\n";
	}
}
#endif

int main() {
	// added the two lines below
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(NULL); 
	{
		auto b = B{};
		auto [x,y] = b;
		std::cout<<"{a:	"<<x<<";b:	"<<y<<";}"<<std::endl;
    }
    {
		auto d1 = D1{};
		auto [x,y] = d1;
		std::cout<<"{a:	"<<x<<";b:	"<<y<<";}"<<std::endl;
    }
    {
		auto d2 = D2{};
		//auto [x,y,z]=d2;//这里不行，
		//auto [{x,y},z]=d2;//这里不行，
		//这样也不行，在c++23之后才有新的方式
		//auto [b,z]=  d2;
		//auto [x,y] = b;
		//虽说c++23支持了反射，但是也很难啊，但是结构体的继承是ECS的标准形式吧
		//std::cout<<"{a:	"<<x<<";b:	"<<y<<";c:	"<<z<<";}"<<std::endl;
    }
    {
		//struct在ECS里面很重要，而且struct的内存分布对ECS的性能也很重要
		// 2026年2月2日，这里要用到c++23相关的反射形式以及struct的继承形式
    }
    {
		//int[] 的解析
		int arr[] ={13,17,19,13};
		//非常不好解析，因为数组的数量运行时确定的
		auto [x,y,z,w] = arr; //copy 形式，如果要是引用就 用auto& 的形式吧
		std::cout<<"["<<x<<","<<y<<","<<z<<","<<w<<"]"<<std::endl;
    }
    {
		//array<int, 4>的解析
		std::array<int,4> arr ={13,17,19,13};
		auto [x,y,z,w] = arr; //copy 形式，如果要是引用就 用auto& 的形式吧 还有其他形式const auto& 等
		std::cout<<"["<<x<<","<<y<<","<<z<<","<<w<<"]"<<std::endl;
    }
    {
		//array<int, 4>的解析
		std::array<int,4> arr ={13,17,19,13};
		auto& [x,y,z,w] = arr; //auto&引用
		w=23;
		std::cout<<"["<<x<<","<<y<<","<<z<<","<<w<<"]"<<std::endl;
		std::cout<<"["<<arr[0]<<","<<arr[1]<<","<<arr[2]<<","<<arr[3]<<"]"<<std::endl;
    }
    {
		//array<int, 4>的解析
		std::array<int,4> arr ={13,17,19,13};
		auto&& [x,y,z,w] = arr; //auto&更强
		w=23;
		std::cout<<"["<<x<<","<<y<<","<<z<<","<<w<<"]"<<std::endl;
		std::cout<<"["<<arr[0]<<","<<arr[1]<<","<<arr[2]<<","<<arr[3]<<"]"<<std::endl;
    }
    {
		//std::tuple 从用例上来看是不知道写了个啥
		auto func=[&](const D2& d)->std::tuple<char,float,std::string>{
			return std::make_tuple(static_cast<char>(d.a+'0'),static_cast<float>(d.b),"c_value: " + std::to_string(d.c));
		};
		auto d2 = D2{};
		auto [x,y,z]=func(d2);
		std::cout<<"{a:	"<<static_cast<int>(x)<<";b:	"<<y<<";c:	"<<z<<";}"<<std::endl;
    }
    {
		//返回tuple的有点，data，result同时返回
		auto func=[&](const D2&d)->std::tuple<const D2&,bool>{
			return std::make_tuple(d,true);
		};
		auto d2 = D2{};
		auto [d,ok]=func(d2);
		if( ok ) std::cout<<"Finish OK!"<<std::endl;
		else std::cout<<"Fail!"<<std::endl;
    }
    {
		//感觉tuple的核心用法就在，增加事件处理的形式了，比如insert之类的操作。改进代码可读性
		//std::map<std::string, int> coll;
		//auto ret = coll.insert({"new", 42});
		//if (!ret.second) //插入失败
		//如果用tuple的话，看起来比较清晰
		std::map<std::string, int> coll;
		auto [pos,ok] = coll.insert({"new",42});//pos是std::pair
		std::cout<<ok<<"	"<<(*pos).second<<"	"<<(*pos).first<<"	"<<std::endl;
    }
    {
		//std::tie的本质，把变量“打包”成一个可赋值的 tuple-like 对象 用法1
		int a = 10, b = 20, c = 30;
		// std::tie(a, b, c) 返回的是：
		// std::tuple<int&, int&, int&> —— 包含对 a,b,c 的引用！
		auto t = std::tie(a, b, c);  // t 是引用包装器，不是拷贝！
		// 现在可以把它当作左值来赋值：
		t = std::make_tuple(100, 200,300);  // ✅ 成功！a=100, b=200, c=300
		std::cout << a << ", " << b << ", " << c << "\n";  // 输出: 100, 200, 300
		//tie的用法，一次赋值所有返回值，感觉如果gl/vulkan的API要能改的话 可以减少调用
		//比如这种的，确实很精确但是确实很烦啊
		// vector<VkQueueFamilyProperties> queueFamilies{};
		// uint32_t queueFamilyCount = 0;
		// vkGetPhysicalDeviceQueueFamilyProperties(dev, &queueFamilyCount, nullptr);//size
		// ✅ 正确：使用 resize() 设置 vector 大小
		// queueFamilies.resize(queueFamilyCount);
		// vkGetPhysicalDeviceQueueFamilyProperties(dev, &queueFamilyCount, queueFamilies.data());
		auto getData = []()->std::tuple<int, double, std::string> {
			return {42, 3.14, "hello"};
		};
		int count;
		double avg;
		std::string msg;
		// 想更新已有变量？用 tie！
		std::tie(count, avg, msg) = getData();  // 一行搞定三变量赋值
		std::cout << count << ", " << avg << ", " << msg << "\n";  
    }
    {
		//std::tie帮我解决了多次unpack的问题
		auto b = B{};
		auto [x,y] = b;
		std::cout<<"{a:	"<<x<<";b:	"<<y<<";}"<<std::endl;
		//第二次解包
		std::tie(x,y)=std::tie(b.a,b.b);
		std::cout<<"{a:	"<<x<<";b:	"<<y<<";}"<<std::endl;
		//或者给struct添加operator std::tuple<int&, int&>()，
		//此时就可以用理想的形式，解包到tie中，也就是增加解包返回值
		//当然operator T()是特殊函数，普通函数也行的,必须是成员函数
		std::tie(x,y)=b;
		std::cout<<"{a:	"<<x<<";b:	"<<y<<";}"<<std::endl;
    }
    //以下是std::pair 结合std::tupe与std::tie的妙写用法
    {
		//No 1：一次性读取data的所有内容
		// 模拟：每次迭代计算三个相关值（如位置、速度、加速度）
		std::vector<std::tuple<double, double, double>> steps = {
			{1.0, 0.5, 0.1},
			{2.0, 0.8, 0.2},
			{3.0, 1.2, 0.3}
		};
		double pos = 0.0, vel = 0.0, acc = 0.0;
		for (const auto& step : steps) {
			// 一次性更新三个状态变量！
			std::tie(pos, vel, acc) = step;
			// 现在 pos/vel/acc 已被更新为当前步的值
			std::cout << pos << ", " << vel << ", " << acc << "\n";
		}
		std::cout <<"忽略第二项 vel"<<"\n";
		for (const auto& step : steps) {
			// 一次性更新三个状态变量！，NOTE 我忽略其中一个
			std::tie(pos, std::ignore, acc) = step;
			// 现在 pos/vel/acc 已被更新为当前步的值
			std::cout << pos << ", " << vel << ", " << acc << "\n";
		}
    }
    //场景 3：算法中交换/更新多个值（比 std::swap 更灵活）
    {
		// 例如：Fibonacci 数列迭代更新，这个不需要历史记忆的，所以算是特殊的情况吧，带dp的都不好用tie来处理
		int a = 0, b = 1;
		for (int i = 0; i < 10; ++i) {
			std::cout << a << " ";
			// 同时更新：a = b, b = a + b
			std::tie(a, b) = std::make_tuple(b, a + b);
		}
		std::cout<<std::endl;
		// 输出: 0 1 1 2 3 5 8 13 21 34
    }
    //经典的用法 boyer_moore_searcher 用法，没有这个特性，代码变得很长
    {
		constexpr std::string_view haystack =
        "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed "
        "do eiusmod tempor incididunt ut labore et dolore magna aliqua";
        const std::string_view needle{"pisci"};
        std::boyer_moore_searcher bmsearch{needle.begin(), needle.end()};
        for(auto [beg,end]=bmsearch(haystack.begin(),haystack.end());
        beg != haystack.end();
        std::tie(beg,end)=bmsearch(end,haystack.end())){
			std::cout << "The string " << std::quoted(needle) << " found at offset "
                  << beg - haystack.begin() << '\n';
        }
        std::cout<<std::endl;
        
    }
    //PERF_START(test1)
    
     //测试代码 - 分配一些内存
    //std::vector<int> vec;
    //for (int i = 0; i < 1000000; i++) {
    //    vec.push_back(i);
    //}
    
    //PERF_END(test1)
    
    //PERF_START(test2)
    
     //更多内存分配
    //std::vector<std::vector<int>> matrix;
    //for (int i = 0; i < 1000; i++) {
    //    matrix.emplace_back(1000, i);
    //}
    
    //PERF_END(test2)
    
    return 0;
}
//cl /EHsc /std:c++17 02TupleLikeElement.cpp /link psapi.lib 