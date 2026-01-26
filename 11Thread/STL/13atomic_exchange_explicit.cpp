#include <atomic>
#include <iostream>
#include <thread>
#include <vector>
 
std::atomic<bool> lock(false); // 锁定时持有 true，解锁时持有 false
 
int new_line{1}; // 访问通过原子锁变量同步
 
void f(int n)
{
    for (int cnt = 0; cnt < 100; ++cnt)
    {
        while (std::atomic_exchange_explicit(&lock, true, std::memory_order_acquire))
            ; // 获取前自旋
        std::cout << n << (new_line++ % 80 ? "" : "\n");
        std::atomic_store_explicit(&lock, false, std::memory_order_release);
    }
}
 
int main()
{
    std::vector<std::thread> v;
    for (int n = 0; n < 8; ++n)
        v.emplace_back(f, n);
    for (auto& t : v)
        t.join();
}
////输出过程相对比较乱序