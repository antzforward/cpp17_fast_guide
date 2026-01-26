#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <mutex>
// 返回实现支持的并发线程数。应该只把该值当做提示。why？
/*
std::thread::hardware_concurrency() 函数返回可用于并发执行的线程数的近似值，这通常对应于系统中的核心数量。然而，这只是一个提示，并不能保证在任何给定的时间点都有这么多的硬件线程可以使用。理由包括：

操作系统可能限制了程序使用的CPU核心数量。
其他进程可能正在使用一些CPU核心，导致实际可用的少于报告的数量。
系统策略可能导致核心在不同任务之间共享或动态调整。
*/
std::mutex g_display_mutex;
int main() {
    unsigned int n = std::thread::hardware_concurrency();
    std::cout << n << " concurrent threads are supported.\n";
    
    // 尝试创建比 hardware_concurrency 更多的线程
    const unsigned int extraThreads = 5;
    unsigned int totalThreads = n + extraThreads;
    
    std::vector<std::thread> threads;

    for (unsigned int i = 0; i < totalThreads; ++i) {
        threads.emplace_back([=] {
			g_display_mutex.lock();
			std::cout << "thread " << i << " sleeping...\n";
			g_display_mutex.unlock();
            std::this_thread::sleep_for(std::chrono::seconds(1));
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    return 0;
}