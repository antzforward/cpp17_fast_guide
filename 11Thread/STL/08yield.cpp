#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <vector>
#include <windows.h>

std::mutex g_display_mutex; 
// 建议其他线程运行一小段时间的“忙睡眠”
void little_sleep(std::chrono::microseconds us)
{
    auto start = std::chrono::high_resolution_clock::now();
    auto end = start + us;
    do {
		std::thread::id this_id = std::this_thread::get_id();//获得当前的id
		g_display_mutex.lock();
		HANDLE hThread = GetCurrentThread(); // 获得当前线程的伪句柄
		int priority = GetThreadPriority(hThread); // 获取优先级
		std::cout << "thread " << this_id <<" with priority :"<<priority  <<" sleeping...\n";
		g_display_mutex.unlock();
        std::this_thread::yield();
    } while (std::chrono::high_resolution_clock::now() < end);
}
void foo(std::chrono::microseconds us){
	auto start = std::chrono::high_resolution_clock::now();
    auto end = start + us;
    do {
        std::this_thread::yield();
    } while (std::chrono::high_resolution_clock::now() < end);
}
 
int main()
{
    auto start = std::chrono::high_resolution_clock::now();
 
    foo(std::chrono::microseconds(100));
 
    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    std::cout << "waited for "
              << std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count()
              << " microseconds\n";
    
    unsigned int n = std::thread::hardware_concurrency();
    std::cout << n << " concurrent threads are supported.\n";
    
    // 尝试创建比 hardware_concurrency 更多的线程
    const unsigned int extraThreads = 0;
    unsigned int totalThreads = 3;
    
    std::vector<std::thread> threads;

    for (unsigned int i = 0; i < totalThreads; ++i) {
        threads.emplace_back( little_sleep, std::chrono::microseconds(1000));
    }

    for (auto& t : threads) {
        t.join();
    }
    
}
//看来yield 并不平衡啊，输出一下windows下的优先级看看吧,无效啊，都是0级的优先级（THREAD_PRIORITY_NORMAL：0）
//发现当thread n为2的时候，一个thread 执行了非常多的次数，另外一个只执行了一次。
//发现当thread n为3的时候，一个thread 执行了非常多的次数，另外两个个只执行了一次。
//从尝试过程来看 yield只是简单方法，不是常规方法。