#include <atomic>
#include <cassert>
#include <thread>
#include <vector>
 
std::vector<int> data;
std::atomic<int> flag = {0};
 
void thread_1()
{
    data.push_back(42);
    flag.store(1, std::memory_order_release);
}
 
void thread_2()
{
    int expected=1;
    // memory_order_relaxed 是可以的，因为这是一个 RMW 操作
    // 而 RMW（以任意定序）跟在释放之后将组成释放序列
    while (!flag.compare_exchange_strong(expected, 2, std::memory_order_relaxed))
    {
        expected = 1;
    }
}
 
void thread_3()
{
    while (flag.load(std::memory_order_acquire) < 2)
        ;
    // 如果我们从 atomic flag 中读到 2，将看到 vector 中储存 42
    assert(data.at(0) == 42); //决不出错
}
 
int main()
{
    std::thread a(thread_1);
    std::thread b(thread_2);
    std::thread c(thread_3);
    a.join(); b.join(); c.join();
}