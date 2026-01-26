#include <iostream>
#include <thread>
#include <chrono>
 /*
 阻塞当前线程直至 *this 所标识的线程结束其执行。
*this 所标识的线程的完成同步于对应的从 join() 成功返回。
*this 自身上不进行同步。同时从多个线程在同一 thread 对象上调用 join() 构成数据竞争，导致未定义行为。
 */
void foo()
{
    // 模拟耗费大量资源的操作
    std::this_thread::sleep_for(std::chrono::seconds(1));
}
 
void bar()
{
    // 模拟耗费大量资源的操作
    std::this_thread::sleep_for(std::chrono::seconds(1));
}
 
int main()
{
    std::cout << "starting first helper...\n";
    std::thread helper1(foo);
 
    std::cout << "starting second helper...\n";
    std::thread helper2(bar);
 
    std::cout << "waiting for helpers to finish..." << std::endl;
    helper1.join();
    helper2.join();
 
    std::cout << "done!\n";
}