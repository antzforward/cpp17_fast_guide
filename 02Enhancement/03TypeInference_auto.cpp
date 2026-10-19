#include <iostream>
#include <vector>
#include <algorithm>

int main(){
	auto i = 5;
	int  arr[10] = {0};
	arr[i] = i;
	std::cout<<"int  arr[10] = {0};arr[i] = i;:"<<"[";
	std::for_each(arr, arr+10, [](const int n) { std::cout << n << ','; });
	std::cout<<"]"<<std::endl;
	
	auto auto_arr = arr; //不能这样？
	
	std::cout<<"auto auto_arr = arr;:\t"<<"[";
	std::for_each(auto_arr, auto_arr+10, [](const int n) { std::cout << n << ','; });
	std::cout<<"]"<<std::endl;
	
	std::cout<<"auto_arr == arr: \t"<<(arr == auto_arr)<<"\t auto_arr pos:"<<auto_arr<<"\tarrpos:"<<arr<<std::endl;
	//auto auto_arr2[10] = {0};//这样可以吗？“auto [10]”: 数组不能具有其中包含“auto”的元素类型
	auto parr = new auto(10);//并不短啊 等同于int* parr= new int(10),一个指针指向保存10的地址。
	std::cout<<"auto parr = new auto(10);:\t parr = \t"<<*parr<<std::endl;
	delete parr;
	/*
	auto parr2 = new auto( i );
	std::cout<<"auto parr2 = new auto( i );  "<<parr2<<std::endl;
	delete parr2;
	*/

	std::vector<int> v={8,4,5,9};
	v.emplace_back(6);
	v.emplace_back(9);
	//移动位置
	auto parr2 = new auto( i );
	std::cout<<"auto parr2 = new auto( i );\t parr2 location = \t"<<parr2<<std::endl;
	delete parr2;
	
	std::cout<<"[";
	std::for_each(v.begin(), v.end(), [](const int n) { std::cout << n << ','; });
	std::cout<<"]"<<std::endl;
	
	std::cout<<"[";
	for(auto itr = v.begin(); itr!= v.end(); ++itr){
		std::cout << *itr << ',';
	}
	std::cout<<"]"<<std::endl;
}