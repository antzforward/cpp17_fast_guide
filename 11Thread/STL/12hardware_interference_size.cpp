#include <atomic>
#include <chrono>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <new>//c++17 放的两个常量。
#include <thread>
//这些常量提供一种可移植的访问 L1 数据缓存线大小的方式。 
#ifdef __cpp_lib_hardware_interference_size
    using std::hardware_constructive_interference_size;
    using std::hardware_destructive_interference_size;
#else
    // 在 x86-64 │ L1_CACHE_BYTES │ L1_CACHE_SHIFT │ __cacheline_aligned │ ... 上为 64 字节
    constexpr std::size_t hardware_constructive_interference_size = 64;
    constexpr std::size_t hardware_destructive_interference_size = 64;
#endif
 
std::mutex cout_mutex;
 
constexpr int max_write_iterations{10'000'000}; // 性能评估时间调节
 /* 程序使用二个线程（原子地）写入给定全局对象的数据成员。第一个对象适合一条缓存线，这导致“硬件干涉”。第二个对象保持其数据成员在分离的缓存线上，故避免了线程写入后可能的“缓存同步”。
 */
struct alignas(hardware_constructive_interference_size)
OneCacheLiner { // 占据一条缓存线
    std::atomic_uint64_t x{};
    std::atomic_uint64_t y{};
} oneCacheLiner;
 
struct TwoCacheLiner { // 占据二条缓存线
    alignas(hardware_destructive_interference_size) std::atomic_uint64_t x{};//alignas的用法
    alignas(hardware_destructive_interference_size) std::atomic_uint64_t y{};
} twoCacheLiner;
 
inline auto now() noexcept { return std::chrono::high_resolution_clock::now(); }//类似using语句
 
template<bool xy>
void oneCacheLinerThread() {
    const auto start { now() };
 
    for (uint64_t count{}; count != max_write_iterations; ++count)
        if constexpr (xy)
             oneCacheLiner.x.fetch_add(1, std::memory_order_relaxed);//fetch_add 不懂
        else oneCacheLiner.y.fetch_add(1, std::memory_order_relaxed);
 
    const std::chrono::duration<double, std::milli> elapsed { now() - start };
    std::lock_guard lk{cout_mutex};
    std::cout << "oneCacheLinerThread() spent " << elapsed.count() << " ms\n";
    if constexpr (xy)
         oneCacheLiner.x = elapsed.count();
    else oneCacheLiner.y = elapsed.count();
}
 
template<bool xy>
void twoCacheLinerThread() {
    const auto start { now() };
 
    for (uint64_t count{}; count != max_write_iterations; ++count)
        if constexpr (xy)
             twoCacheLiner.x.fetch_add(1, std::memory_order_relaxed);
        else twoCacheLiner.y.fetch_add(1, std::memory_order_relaxed);
 
    const std::chrono::duration<double, std::milli> elapsed { now() - start };
    std::lock_guard lk{cout_mutex};
    std::cout << "twoCacheLinerThread() spent " << elapsed.count() << " ms\n";
    if constexpr (xy)
         twoCacheLiner.x = elapsed.count();
    else twoCacheLiner.y = elapsed.count();
}
 
int main() {
    std::cout << "__cpp_lib_hardware_interference_size "
#   ifdef __cpp_lib_hardware_interference_size
        " = " << __cpp_lib_hardware_interference_size << "\n";
#   else
        "is not defined, use 64 as fallback\n";
#   endif
 
    std::cout
        << "hardware_destructive_interference_size == "
        << hardware_destructive_interference_size << '\n'
        << "hardware_constructive_interference_size == "
        << hardware_constructive_interference_size << "\n\n";
 
    std::cout
        << std::fixed << std::setprecision(2)
        << "sizeof( OneCacheLiner ) == " << sizeof( OneCacheLiner ) << '\n'
        << "sizeof( TwoCacheLiner ) == " << sizeof( TwoCacheLiner ) << "\n\n";
 
    constexpr int max_runs{4};
 
    int oneCacheLiner_average{0};
    for (auto i{0}; i != max_runs; ++i) {
        std::thread th1{oneCacheLinerThread<0>};
        std::thread th2{oneCacheLinerThread<1>};
        th1.join(); th2.join();
        oneCacheLiner_average += oneCacheLiner.x + oneCacheLiner.y;
    }
    std::cout << "Average time: " << (oneCacheLiner_average / max_runs / 2) << " ms\n\n";
 
    int twoCacheLiner_average{0};
    for (auto i{0}; i != max_runs; ++i) {
        std::thread th1{twoCacheLinerThread<0>};
        std::thread th2{twoCacheLinerThread<1>};
        th1.join(); th2.join();
        twoCacheLiner_average += twoCacheLiner.x + twoCacheLiner.y;
    }
    std::cout << "Average time: " << (twoCacheLiner_average / max_runs / 2) << " ms\n\n";
}
/* 
std::hardware_destructive_interference_size 和 std::hardware_constructive_interference_size 是 C++17 引入的两个常量表达式，位于 <new> 头文件中。它们提供了有关底层硬件缓存系统的信息：

std::hardware_destructive_interference_size: 表示在同一缓存行上至少需要多少字节的间隔来防止两个对象之间的假共享（False Sharing）。假共享是指多个处理器核心在没有必要时无效地使对方的缓存行失效，这会降低并发程序的性能。
std::hardware_constructive_interference_size: 表示两个或多个对象可以无干扰地位于同一缓存行上的最大大小。
为什么设计它们？

在多核处理器上编写并发代码时，理解和利用缓存的行为非常重要。当多个线程访问相邻的内存位置时，如果数据落在同一个缓存行上，可能会导致不必要的性能开销，因为缓存行可能会在核之间频繁传递（即假共享）。

如何使用？

确保并发访问的变量不会造成假共享，你可以将它们分开放置，至少相隔 std::hardware_destructive_interference_size 的距离。
*/