//所有std::atomic<整数>的别名，包括了所有整数类型的写法。比如atomic_int等。
#include <atomic>
#include <iostream>
#include <thread>
#include <vector>
 
std::atomic_int acnt;
int cnt;
 
void f()
{
    for (int n = 0; n < 10000; ++n)
    {
        ++acnt;
        ++cnt;
        // 注意: 对于此示例，宽松内存定序已经足够，
        // 例如 acnt.fetch_add(1, std::memory_order_relaxed);
    }
}
 
int main()
{

	std::vector<std::thread> pool;//jthread是C++20的内容，换成thread，然后手动join
	for (int n = 0; n < 10; ++n)
		pool.emplace_back(f);

	
	for (auto& th : pool)
        if (th.joinable())
            th.join();

    std::cout << "原子计数器为 " << acnt << '\n'
              << "非原子计数器为 " << cnt << '\n';
}
/* 输出结果
原子计数器为 100000
非原子计数器为 84602 //数量少于10000，因为有些++操作覆盖了。
*/