#include <iostream>
#include <vector>
#include <map>
#include <variant>
#include <type_traits>
#include <string>
#include <optional> //返回值，不抛出异常的形式
#include <memory> //unique_ptr 等
#include <utility> //std::pair
#include "../TestMacro.h"
/**
 * @file 21PythonWalrusOpLike.cpp
 * @ingroup 01Basics
 * @brief C++17 带初始化的 switch 语句 — switch(init; expr) 的各种用法
 * @details 对应 C++17 `switch(initialization; expression)` 语法，
 *          将变量初始化和 switch 判断合并，变量作用域限制在 switch 块内。
 *
 *          覆盖场景：
 *          - 基本的 switch + 初始化
 *          - 配合 std::variant 的类型分发
 *          - 配合结构化绑定解包 map::insert 返回值
 *          - tuple 解包作为 switch 初始化语句
 *          - std::visit 模式替代复杂 switch（C++17 引入）
 *          - 配合 constexpr if 的类型判断
 */
// Python := Walrus Operator（海象运算符，因为它看起来就是一个海象旋转了90°）
// 当需要同一个表达式中同时使用和测试一个值的时候，它有助于避免冗余代码--尤其是在循环或者条件语句中。
// 在c++中，这种用法叫做带初始化的if和switch语句，c++的for语句自然就有这个初始化过程，如果while不好写，用for语句吧
// 注意变量的生命周期，c++ 和 Python都是在对应语句范围内，c# 基本是个语法糖
// 这一章，主要讲带init的switch 语句的情况，书上的例子缺乏单文件运行性，所以自己找例子了。
// switch语句的格式是 switch(initilaization_expr;conditional_expr)
// 除了少数的语言不支持switch，很多语言都支持，不过多个语言来看，我还是比较喜欢c#的switch
int main() {
	// added the two lines below
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(NULL); 
	std::cout<<"基础用法，switch(initilaization_expr;conditional_expr)\n" ;
    {
		auto get_status_code = []() { return 404; };
		switch (int code = get_status_code(); code) {
        case 200:
            std::cout << "Success: " << code << std::endl;
            break;
        case 404:
            std::cout << "Not Found: " << code << std::endl;
            break;
        case 500:
            std::cout << "Server Error: " << code << std::endl;
            break;
        default:
            std::cout << "Unknown: " << code << std::endl;
            break;
		}
    }
    std::cout<<"与variant 结合使用\n";
    {
		// 与variant结合使用
		std::variant<int, std::string, double> var = std::string("hello");
		
		switch (auto&& v = var; v.index()) {
			case 0: // int
				std::cout << "Integer: " << std::get<int>(v) << std::endl;
				break;
			case 1: // string
				std::cout << "String: " << std::get<std::string>(v) << std::endl;
				break;
			case 2: // double
				std::cout << "Double: " << std::get<double>(v) << std::endl;
				break;
		}
    }
    std::cout<<"与结构化绑定\n";
    {
		 std::map<std::string, int> data = {{"key1", 100}, {"key2", 200}};
    
		if (auto [it, found] = data.insert({"new_key", 300}); found) {
			std::cout << "Inserted: " << it->first << " = " << it->second << std::endl;
		}
    }
    std::cout<<"使用初始化捕获变量\n";
    {
		std::string input = "hello";
		switch (char firstChar = input.empty() ? '\0' : input[0]; firstChar) {
			case 'h':
				std::cout << "Starts with 'h'" << std::endl;
				break;
			case 'w':
				std::cout << "Starts with 'w'" << std::endl;
				break;
			default:
				std::cout << "Starts with '" << firstChar << "'" << std::endl;
		}
    }
    std::cout<<"tuple 解析作为初始化语句的情况\n";
    {
		std::pair<int, std::string> data {3, "test"};
		switch (auto [num, str] = data; num) {
			case 1:
				std::cout << "num=1, str=" << str << std::endl;
				break;
			case 3:
				std::cout << "num=3, str=" << str << std::endl;
				break;
		}
    }
    std::cout<<"配合constexpr if风格（概念上）我用c++17 也行 \n";
    {
		auto process_value = [](auto val) {
			//// 使用if constexpr替代多个switch的常见模式
			if constexpr (std::is_integral_v<decltype(val)>) {
				std::cout << "Integral value: " << val << std::endl;
			} else if constexpr (std::is_floating_point_v<decltype(val)>) {
				std::cout << "Floating point value: " << val << std::endl;
			}
		};
		
		process_value(42);
		process_value(3.14);
    }
    std::cout<<"※※使用std::variant的visit模式（替代复杂switch）\n";
    std::cout<<"※※使用std::variant（C++17引入，C++20改进）\n";
    {
		std::variant<int, double, std::string> var = 3.14159;
		std::visit([](auto&& arg) {
			using T = std::decay_t<decltype(arg)>;
			if constexpr (std::is_same_v<T, int>) {
				std::cout << "int: " << arg << std::endl;
			} else if constexpr (std::is_same_v<T, double>) {
				std::cout << "double: " << arg << std::endl;
			} else if constexpr (std::is_same_v<T, std::string>) {
				std::cout << "string: " << arg << std::endl;
			}
		}, var);
    }
    std::cout<<"带初始化的switch实际应用\n";
    {
		enum class ErrorCode {
			OK,
			FILE_NOT_FOUND,
			PERMISSION_DENIED,
			UNKNOWN_ERROR
		};
		
		auto get_error_message = [](ErrorCode code) -> std::string {
			switch (code) {
				case ErrorCode::OK: return "Success";
				case ErrorCode::FILE_NOT_FOUND: return "File not found";
				case ErrorCode::PERMISSION_DENIED: return "Permission denied";
				default: return "Unknown error";
			}
		};
		
		// 模拟一个可能失败的操作
		auto operation_result = []() -> std::pair<ErrorCode, std::string> {
			return {ErrorCode::PERMISSION_DENIED, "config.txt"};
		};
		
		// 使用带初始化的switch处理结果
		switch (auto [error, filename] = operation_result(); error) {
			case ErrorCode::OK:
				std::cout << "Operation succeeded on " << filename << std::endl;
				break;
			case ErrorCode::FILE_NOT_FOUND:
				std::cout << "File not found: " << filename << std::endl;
				break;
			case ErrorCode::PERMISSION_DENIED:
				std::cout << "Permission denied for: " << filename 
						  << " - " << get_error_message(error) << std::endl;
				break;
			default:
				std::cout << get_error_message(error) << std::endl;
		}
    }
    std::cout<<"C++20中switch的重要注意事项\n";
    {
		// 1. 初始化部分的作用域仅限于整个switch
		int external = 10;
		switch (int internal = external * 2; internal) {
			case 20:
				// 这里可以访问internal
				std::cout << "internal = " << internal << std::endl;
				// 也可以访问external
				std::cout << "external = " << external << std::endl;
				break;
		}
		// 这里不能访问internal
		// std::cout << internal << std::endl; // 错误！
		
		// 2. 初始化部分可以是复杂表达式
		std::string status = "active";
		switch (auto len = status.length(); 
				status == "active" ? 1 : 
				status == "inactive" ? 2 : 0) {
			case 1:
				std::cout << "Active, length=" << len << std::endl;
				break;
			case 2:
				std::cout << "Inactive, length=" << len << std::endl;
				break;
			default:
				std::cout << "Unknown, length=" << len << std::endl;
		}
		
		// 3. 可以和if语句对比（C++17后if也有初始化）
		if (int x = 5; x > 3) {
			std::cout << "if with init: x=" << x << std::endl;
		}
    }
    return 0;
}
//cl /EHsc /std:c++20 21PythonWalrusOpLike.cpp /link psapi.lib 