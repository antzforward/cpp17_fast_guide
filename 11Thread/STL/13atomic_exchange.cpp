#include <algorithm>
#include <atomic>
#include <cstddef>
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
//以 desired 原子地替换底层值。操作为读-修改-写操作。根据 order 的值影响内存。返回值是：原子变量在调用前的值
//因为osyncstream是c++20的内容 要改动一下

int main()
{
    constexpr std::size_t  thread_count{5};
    constexpr int sum{5};
 
    std::atomic<int> atom{0};
    std::atomic<int> counter{0};
	std::mutex cout_mutex;
	
    auto increment_to_sum = [&](const std::size_t id)
    {
        for (int next = 0; next < sum;)
        {
            // 每个线程各自写入自身所知的值
            const int current = atom.exchange(next);//但是没有memory order的设定。
            counter++;
            // 同步写入，避免被其他线程中断
            cout_mutex.lock();
            std::cout<< "线程 #" << id << "（id=" << std::this_thread::get_id() << "）写入 " << next << "，替换旧值 "<< current << "。\n";
            cout_mutex.unlock();
            next = std::max(current, next) + 1;
        }
    };
 
    std::vector<std::thread> v;
    for (std::size_t i = 0; i != thread_count; ++i)
        v.emplace_back(increment_to_sum, i);
 
    for (auto& tr : v)
        tr.join();
 
    std::cout << thread_count << " 个线程总共用了 "
              << counter << " 次将 0 增加到 " << sum << "。\n";
}
