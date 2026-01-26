#include <iostream>
#include <utility>

void reference(int& v) {
	std::cout << "左值" << std::endl;
}
void reference(int&& v) {
	std::cout << "右值" << std::endl;
}
template <typename T>
void pass(T&& v) {
	std::cout << "普通传参:";
	reference(v);
	std::cout << "std::move 传参:";
	reference(std::move(v));
	std::cout << "std::forward 传参:";
	reference(std::forward<T>(v));//这里的T的用法有奇怪吧，传入右值，forward的类型匹配为T，测试了一下 T与T&&结果相同。T&结果不同
}
int main() {
	std::cout << "传递右值:" << std::endl;
	pass(1); // 1是右值, 但输出左值
	std::cout << "传递左值:" << std::endl;
	int v = 1;
	pass(v); // r 是左引用, 输出左值
	return 0;
}