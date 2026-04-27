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
/**
 * @file 02TupleLikeElement.cpp
 * @ingroup 01Basics
 * @brief C++17 结构化绑定与 std::tie — tuple-like 对象解包详解
 * @details 本示例覆盖以下内容：
 *   -# 结构化绑定（structured bindings）：C++17 的核心特性，允许对 tuple-like 对象进行解包
 *      支持：struct/class、原生数组、std::array、std::tuple、std::pair 等
 *      语法：`auto [a, b, c] = expr;` 将 expr 的各个成员绑定到 a, b, c
 *   -# std::tie 的用法：将已有变量"打包"成引用 tuple，实现多变量同时赋值
 *      核心场景：多返回值解包、忽略部分返回值（`std::ignore`）、同时更新多个变量
 *   -# 继承形式 struct 的解包，如何通过 `operator std::tuple` 转换来支持结构化绑定
 *   -# array 的解包：由于需要编译期确定元素数量，无法像 Python 切片那样灵活（首尾/步长）
 */

/**
 * @brief 基类结构体，包含两个 int 成员和一个到 tuple 的转换运算符
 *
 * 通过 `operator std::tuple<int&, int&>()` 将自身转换为 tuple-like 对象，
 * 使得结构化绑定可以对 B 进行解包：`auto [x, y] = b;`
 */
struct B{
	int a = 1;  ///< 整型成员 a
	int b = 2;  ///< 整型成员 b
	operator std::tuple<int&, int&>() {
		return std::tie(a, b);
	}
};
/**
 * @brief 派生类 D1，继承自 B，无新增成员
 *
 * 结构化绑定仍然按照 B 的格式解包（a, b）。
 */
struct D1:B{
};
/**
 * @brief 派生类 D2，继承自 B 并新增成员 c
 *
 * 重新定义了 `operator std::tuple<int&, int&, int&>()`，支持解包（a, b, c）。
 * @note C++17 结构化绑定无法直接对带继承的派生类解包全部成员，必须通过转换运算符。
 */
struct D2:B{
	int c = 3;  ///< D2 新增的整型成员 c
	operator std::tuple<int&, int&,int&>() {
		return std::tie(a, b,c);
	}
};

#if CPP17
// if constexpr + is_same_v 的用法：编译期根据类型选择不同的分支（注意：这里使用了编译不过的 is_based_of_v，仅作为示意）
template<typename Derived,typename Base>
constexpr bool is_base_of_v = std::is_based_of_v<Base,Derived>;
// 编译期分析 struct 的类型信息，输出不同类型的特征
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
	// 提高 I/O 性能
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(NULL);
	// ===== 基本 struct 解包 =====
	{
		// B 是一个聚合体，结构化绑定可以直接解包其成员 a, b
		auto b = B{};
		auto [x,y] = b;  // x 绑定到 a，y 绑定到 b
		std::cout<<"{a:	"<<x<<";b:	"<<y<<";}"<<std::endl;
    }
    {
		// D1 继承自 B，结构化绑定仍然按照 B 的格式解包（a, b）
		auto d1 = D1{};
		auto [x,y] = d1;
		std::cout<<"{a:	"<<x<<";b:	"<<y<<";}"<<std::endl;
    }
    {
		// D2 继承自 B 并增加了成员 c，但 C++17 结构化绑定无法直接对带继承的派生类解包全部成员
		auto d2 = D2{};
		// 以下尝试均会编译失败——结构化绑定要求编译期确定元素数量，而 D2 的基类成员无法被展开
		//auto [x,y,z]=d2;       // 不行：D2 不是聚合体（继承导致）
		//auto [{x,y},z]=d2;     // 不行：不支持嵌套解包
		//auto [b,z]=  d2;       // 不行：无法部分解包
		//auto [x,y] = b;        // 不行：b 未定义
		// TODO: C++23 引入了反射机制，可以实现对继承 struct 的完全解包（虽然语法较复杂）
		//std::cout<<"{a:	"<<x<<";b:	"<<y<<";c:	"<<z<<";}"<<std::endl;
    }
    {
		// 在 ECS 架构中 struct 非常重要，struct 的内存布局直接影响缓存性能
		// TODO: 2026年2月2日，需要结合 C++23 反射与 struct 继承形式来完善解包
    }
    {
		// ===== 原生数组的结构化绑定 =====
		// C++17 允许对固定大小数组进行结构化绑定，但元素数量必须在编译期已知
		// 局限性：无法像 Python 那样用切片（如 arr[1:3]）灵活取子集
		int arr[] ={13,17,19,13};
		auto [x,y,z,w] = arr; // 解包为 4 个独立变量（值拷贝形式）
		std::cout<<"["<<x<<","<<y<<","<<z<<","<<w<<"]"<<std::endl;
    }
    {
		// ===== std::array 的结构化绑定（值拷贝） =====
		// std::array 天然支持结构化绑定，行为与原生数组一致
		// 如果需要引用绑定，使用 auto& 或 const auto&
		std::array<int,4> arr ={13,17,19,13};
		auto [x,y,z,w] = arr; // 值拷贝形式
		std::cout<<"["<<x<<","<<y<<","<<z<<","<<w<<"]"<<std::endl;
    }
    {
		// ===== std::array 的结构化绑定（引用形式） =====
		// 使用 auto& 绑定引用，对绑定变量的修改会反映到原数组
		std::array<int,4> arr ={13,17,19,13};
		auto& [x,y,z,w] = arr; // 引用绑定
		w=23; // 修改 w 即修改 arr[3]
		std::cout<<"["<<x<<","<<y<<","<<z<<","<<w<<"]"<<std::endl;
		std::cout<<"["<<arr[0]<<","<<arr[1]<<","<<arr[2]<<","<<arr[3]<<"]"<<std::endl;
    }
    {
		// ===== std::array 的结构化绑定（万能引用形式） =====
		// auto&& 是万能引用（forwarding reference），可以接受左值和右值
		std::array<int,4> arr ={13,17,19,13};
		auto&& [x,y,z,w] = arr; // 万能引用绑定
		w=23;
		std::cout<<"["<<x<<","<<y<<","<<z<<","<<w<<"]"<<std::endl;
		std::cout<<"["<<arr[0]<<","<<arr[1]<<","<<arr[2]<<","<<arr[3]<<"]"<<std::endl;
    }
    {
		// ===== std::tuple 作为函数多返回值 =====
		// 将 D2 的成员转换为 (char, float, string) 形式的 tuple 返回，实现类型转换 + 多值返回
		auto func=[&](const D2& d)->std::tuple<char,float,std::string>{
			return std::make_tuple(static_cast<char>(d.a+'0'),static_cast<float>(d.b),"c_value: " + std::to_string(d.c));
		};
		auto d2 = D2{};
		auto [x,y,z]=func(d2); // 结构化绑定解包函数返回的 tuple
		std::cout<<"{a:	"<<static_cast<int>(x)<<";b:	"<<y<<";c:	"<<z<<";}"<<std::endl;
    }
    {
		// ===== tuple 返回 data + result 的模式 =====
		// 常见模式：同时返回数据和操作结果（类似 Go 的 (value, err) 风格）
		auto func=[&](const D2&d)->std::tuple<const D2&,bool>{
			return std::make_tuple(d,true);
		};
		auto d2 = D2{};
		auto [d,ok]=func(d2); // d 引用原始数据，ok 标识是否成功
		if( ok ) std::cout<<"Finish OK!"<<std::endl;
		else std::cout<<"Fail!"<<std::endl;
    }
    {
		// ===== std::map::insert 的结构化绑定 =====
		// map.insert() 返回 pair<iterator, bool>，用结构化绑定代替 .first/.second 更清晰
		std::map<std::string, int> coll;
		auto [pos,ok] = coll.insert({"new",42}); // pos 是迭代器，ok 表示是否插入成功
		std::cout<<ok<<"	"<<(*pos).second<<"	"<<(*pos).first<<"	"<<std::endl;
    }
    {
		// ===== std::tie 的核心用法 =====
		// tie(a, b, c) 返回 tuple<int&, int&, int&>，是左值引用的包装器（非拷贝）
		// 赋值操作 = 会将右值 tuple 的每个元素写入对应的引用变量
		int a = 10, b = 20, c = 30;
		auto t = std::tie(a, b, c);  // t 持有对 a, b, c 的引用
		t = std::make_tuple(100, 200,300);  // 通过赋值同时更新 a=100, b=200, c=300
		std::cout << a << “, “ << b << “, “ << c << “\n”;  // 输出: 100, 200, 300
		// tie 典型应用：将已有变量与函数多返回值绑定（避免 auto [x,y,z] 每次都创建新变量）
		// Vulkan/GL 等 C 风格 API 常返回多个值，若改用 tuple 返回可减少样板代码
		auto getData = []()->std::tuple<int, double, std::string> {
			return {42, 3.14, “hello”};
		};
		int count;
		double avg;
		std::string msg;
		std::tie(count, avg, msg) = getData();  // 一行同时赋值三个变量
		std::cout << count << “, “ << avg << “, “ << msg << “\n”;
    }
    {
		// ===== 通过 operator std::tuple 支持反复解包 =====
		// 如果 struct 定义了 operator std::tuple<T&, T&>()，就可以用 tie 反复解包
		// 这比每次写 auto [x,y] 更灵活，适合需要多次更新同一组变量的场景
		auto b = B{};
		auto [x,y] = b;
		std::cout<<”{a:	“<<x<<”;b:	“<<y<<”;}”<<std::endl;
		// 第二次解包：直接通过 tie + 转换运算符
		std::tie(x,y)=std::tie(b.a,b.b);
		std::cout<<”{a:	“<<x<<”;b:	“<<y<<”;}”<<std::endl;
		// 或者利用 operator std::tuple<int&, int&>() 隐式转换，省去手动 tie 成员
		std::tie(x,y)=b; // b 隐式转换为 tuple<int&, int&>
		std::cout<<”{a:	“<<x<<”;b:	“<<y<<”;}”<<std::endl;
    }
    // ===== tie 高级用法 =====
    {
		// 场景 1：遍历 tuple 容器时，用 tie 一次性解包每行数据
		std::vector<std::tuple<double, double, double>> steps = {
			{1.0, 0.5, 0.1},
			{2.0, 0.8, 0.2},
			{3.0, 1.2, 0.3}
		};
		double pos = 0.0, vel = 0.0, acc = 0.0;
		for (const auto& step : steps) {
			std::tie(pos, vel, acc) = step; // 每次迭代同时更新 pos, vel, acc
			std::cout << pos << ", " << vel << ", " << acc << "\n";
		}
		std::cout <<"忽略第二项 vel"<<"\n";
		for (const auto& step : steps) {
			// 场景 2：std::ignore 占位符——忽略不需要的返回值
			std::tie(pos, std::ignore, acc) = step; // 跳过第二个元素，只更新 pos 和 acc
			std::cout << pos << ", " << vel << ", " << acc << "\n";
		}
    }
    // 场景 3：用 tie + make_tuple 实现无临时变量的多值交换
    {
		// 典型应用：Fibonacci 数列迭代——a 和 b 在同一行同时更新
		// 优势：不需要引入临时变量（比三变量 swap 更简洁）
		// 局限：不适合有依赖链的 DP 场景
		int a = 0, b = 1;
		for (int i = 0; i < 10; ++i) {
			std::cout << a << " ";
			std::tie(a, b) = std::make_tuple(b, a + b); // a=旧b, b=旧a+旧b
		}
		std::cout<<std::endl;
		// 输出: 0 1 1 2 3 5 8 13 21 34
    }
    // ===== 实战：boyer_moore_searcher 结合结构化绑定 =====
    // std::boyer_moore_searcher::operator() 返回 pair<iterator, iterator>（匹配起始位置，匹配结束位置）
    // 使用结构化绑定 [beg, end] 直接解包，避免手动写 .first/.second，代码更简洁
    {
		constexpr std::string_view haystack =
        "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed "
        "do eiusmod tempor incididunt ut labore et dolore magna aliqua";
        const std::string_view needle{"pisci"};
        std::boyer_moore_searcher bmsearch{needle.begin(), needle.end()};
        // for 循环利用 tie 解包 beg/end，并从上次匹配的 end 位置继续搜索
        for(auto [beg,end]=bmsearch(haystack.begin(),haystack.end());
        beg != haystack.end();
        std::tie(beg,end)=bmsearch(end,haystack.end())){
			std::cout << "The string " << std::quoted(needle) << " found at offset "
                  << beg - haystack.begin() << '\n';
        }
        std::cout<<std::endl;
        
    }
    // ===== 性能测试代码（已注释） =====
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
// 编译命令: cl /EHsc /std:c++17 02TupleLikeElement.cpp /link psapi.lib