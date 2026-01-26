#include <iostream> //thread safe?
#include <thread> 
#include <vector>
#include <mutex>
void foo(){
	static int id=0;
	std::cout<<"Hello World :"<<id++<<std::endl;//有时候会出现不同
}

int main(){
	std::thread t(foo);
	t.join();
	
	std::vector<std::thread> threads;
	std::mutex cout_mutex; // 创建互斥量用于同步输出
	threads.reserve(5);//预分配内存
	for(int i=0; i<5;i++){
		threads.emplace_back([=,&cout_mutex](){// 捕获互斥量的引用
		std::lock_guard<std::mutex> guard(cout_mutex); // 锁定互斥量
			std::cout<<"Hello World :"<<i<<std::endl;
		});
	}
	for (auto& thread : threads) {
		if (thread.joinable()) {
			thread.join();
		}
	}
	
	return 0;
}