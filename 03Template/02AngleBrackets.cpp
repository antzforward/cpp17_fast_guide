#include <iostream>
#include <vector>
using namespace std;
template<bool T>struct SuckType{};
int main(){
	//传统的情况下>>一律被当成右移符号了，这里要写的分开一点>> => > > 
	vector<vector<int>> mtx = {{1,2,3},{4},{5,6},{7,8,9,10}};
	
	
	cout<<"vector<vector<int>> mtx {";
	for (const auto& inner_vec : mtx) {
		std::cout << "{";
		for (size_t i = 0; i < inner_vec.size(); ++i) {
			std::cout << inner_vec[i];
			if (i < inner_vec.size() - 1) {
				std::cout << ", ";
			}
		}
		std::cout << "}, ";
	}
	std::cout << "\b\b}\n"; // 删除最后一个逗号和空格
	
	vector<SuckType<(1>2)>> v;//这也是合法的。标识type的数值为false。注意template<bool T> SuckType;是不行的。要确定大小。
}