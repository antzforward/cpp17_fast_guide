#include <thread>
#include <mutex>
#include <iostream>
#include <chrono>
#include <windows.h>

std::mutex iomutex;
void f(int num)
{
    std::this_thread::sleep_for(std::chrono::seconds(1));

    {
        std::lock_guard<std::mutex> lk(iomutex);
        HANDLE hThread = GetCurrentThread(); // 获得当前线程的伪句柄
		int priority = GetThreadPriority(hThread); // 获取优先级
        std::cout << "Thread " << num << " is executing at priority "
				 <<priority<<'\n';
    }
}
//SetThreadPriority(hThread, THREAD_PRIORITY_HIGHEST) 第二个参数的内容
/*
截至最后更新的知识（2023年），THREAD_PRIORITY_HIGHEST 的数值通常定义为 2。这里是全部标准的线程优先级，从低到高：

THREAD_PRIORITY_IDLE：-15
THREAD_PRIORITY_LOWEST：-2
THREAD_PRIORITY_BELOW_NORMAL：-1
THREAD_PRIORITY_NORMAL：0
THREAD_PRIORITY_ABOVE_NORMAL：1
THREAD_PRIORITY_HIGHEST：2
THREAD_PRIORITY_TIME_CRITICAL：15
*/
int main()
{
    std::thread t1(f, 1), t2(f, 2);

    HANDLE hThread = static_cast<HANDLE>(t1.native_handle());
    if (!SetThreadPriority(hThread, THREAD_PRIORITY_HIGHEST)) {
        std::lock_guard<std::mutex> lk(iomutex);
        std::cout << "Failed to set thread priority: " << GetLastError() << '\n';
    }

    t1.join();
    t2.join();
}