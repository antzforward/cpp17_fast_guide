#include <iostream>
#include <thread>
#include <chrono>
 
void foo()
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "Task 1 is running" << std::endl;
}
 
void bar()
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "Task 2 is running" << std::endl;
}
 
int main()
{
    std::thread t1(foo);
    std::thread t2(bar);
 
    std::cout << "thread 1 id: " << t1.get_id() << '\n'
              << "thread 2 id: " << t2.get_id() << '\n';
 
    std::swap(t1, t2);//特化的swap，作用一样。
 
    std::cout << "after std::swap(t1, t2):" << '\n'
              << "thread 1 id: " << t1.get_id() << '\n'
              << "thread 2 id: " << t2.get_id() << '\n';
 
    t1.swap(t2);
 
    std::cout << "after t1.swap(t2):" << '\n'
              << "thread 1 id: " << t1.get_id() << '\n'
              << "thread 2 id: " << t2.get_id() << '\n';
 
    // 这里的 join 是为了确保主线程等待两个线程都完成。
    // 在交换后，要加入原来 thread2 的线程现在需要通过 thread1.join() 来加入，
    // 而原来 thread1 的线程需要通过 thread2.join() 来加入。
    if (t1.joinable()) {
        t1.join();
    }

    if (t2.joinable()) {
        t2.join();
    }
}
/*
thread 1 id: 52172
thread 2 id: 38924
after std::swap(t1, t2):
thread 1 id: 38924
thread 2 id: 52172
after t1.swap(t2):
thread 1 id: 52172
thread 2 id: 38924
Task 2 is running
Task 1 is running
*/