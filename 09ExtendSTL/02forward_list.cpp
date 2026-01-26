/*forward list 单link的链表，没有办法获得size，单link容易出现环，因为不直接操作指针，所以用forward list就不会出现环解构了。
std::forward_list 是支持从容器中的任何位置快速插入和移除元素的容器。不支持快速随机访问。它实现为单链表，且实质上与其在 C 中的实现相比无任何开销。与 std::list 相比，此容器在不需要双向迭代时提供更好的存储空间效率。
front 访问第一个元素。
*/
#include <forward_list>
#include <iostream>
#include <string>

int main(){
	std::forward_list<int> flist = {1,2,3,4,5,6,7};
	auto size = std::distance(flist.begin(),flist.end());
	// 输出大小
	std::cout << "Size of the forward_list: " << size << std::endl;
	flist.push_front(8);
	flist.push_front(9);
	auto print=[&](){
	std::cout<<"当前的单向list:\t";
	for(const auto& value:flist){
		std::cout<<value<<' ';
	}
	std::cout<<std::endl;};
	print();
	flist.pop_front();
	print();
	
	flist.assign(5,flist.front()*5);//assign(count,value)
	print();
	flist.clear();
	print();
}