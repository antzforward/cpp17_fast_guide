#include <iostream>
#include <string>

// 字符串字面量自定义必须设置如下的参数列表
std::string operator"" _wow1(const char *wow1, size_t len) {
	return std::string(wow1)+"woooooooooow, amazing";
}
std::string operator"" _wow2 (unsigned long long i) {
	return std::to_string(i)+"woooooooooow, amazing";
}

int main() {
	std::string str2 = R"(C:\What\The\Fxxk)";
	std::cout << str2 << std::endl;
	auto str = "abc"_wow1;
	auto num = 1_wow2;
	std::cout << str << std::endl;
	std::cout << num << std::endl;
	return 0;
}
/*自定义字面量支持四种字面量
1、整型字面量：重载时必须使用unsigned long long、const char* 、模板字面算符参数。如上面的例子。
2、浮点数字面量：重载时必须使用long double、const char * 模板字面量算符
3、字符串字面量：必须使用(const char*,size_t) 形式的参数列表。如上面的例子
4、字符字面量：参数只能是char,wchar_t,char16_t,char32_t 这几种类型。
*/