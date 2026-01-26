#include <iostream>

//trailing return type,c11
template<typename T, typename U>
auto add(T x, U y)->decltype(x+y){
	return x + y;
}

template<typename T, typename U>
auto add14(T x, U y){
	return x+y;
}

int main(){
	std::cout<<"C++11 version: "<<add(36.5f, 14)<<std::endl;
	std::cout<<"C++14 version: "<<add14(36.5f, 14)<<std::endl;
	std::cout<<"C++14 version: "<<[](auto x,auto y){return x+y;}(36, 14)<<std::endl;
	return 0;
}