#include <iostream> //thread safe?
#include <thread> 
#include <vector>
#include <chrono>
#include <mutex>

void foo(){
	static int id=0;
	static std::mutex mutex;
	std::lock_guard<std::mutex> lock(mutex);
	std::cout<<"Hello World :"<<id++<<std::endl;//有时候会出现不同
}

int main(){
	unsigned int n = std::thread::hardware_concurrency();
	std::cout << n << " concurrent threads are supported.\n";
	std::thread t(foo);
	t.join();
	
	std::vector<std::thread> threads;
	threads.reserve(n);//预分配内存
	for(int i=0; i<n;i++){
		threads.emplace_back([=](){
			//std::cout<<"Hello World :"<<i<<std::endl;//有时候窜行，可以看到cout不是thread safe的。用锁来处理吧。
			// 模拟耗费大量资源的操作
			foo();
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
		);
	}
	for (auto& thread : threads) {
		if (thread.joinable()) {
			thread.join();
		}
	}
	
	return 0;
}