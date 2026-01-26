#include <iostream>
#include <thread>
#include <chrono>
 
void foo()
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
}
/* 检查 std::thread 对象是否标识活跃的执行线程。具体而言，若 get_id() != std::thread::id() 则返回 true 。故默认构造的 thread 不可结合。

结束执行代码，但仍未结合的线程仍被当作活跃的执行线程，从而可结合。
*/ 
int main()
{
    std::thread t;
    std::cout << "before starting, joinable: " << std::boolalpha << t.joinable()
              << '\n';
 
    t = std::thread(foo);
    std::cout << "after starting, joinable: " << t.joinable() 
              << '\n';
 
    t.join();
    std::cout << "after joining, joinable: " << t.joinable() 
              << '\n';
}