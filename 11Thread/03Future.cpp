#include <iostream>
#include <future>
#include <thread>

// 这个函数模拟了一个可能会耗时较长的操作
int complex_calculation() noexcept {
    std::this_thread::sleep_for(std::chrono::seconds(2)); // 模拟耗时
    return 42; // 假设这是计算的结果
}

int main() {
    // 启动一个异步任务
    std::future<int> result = std::async(complex_calculation);

    // 主线程可以继续做其他事情
    std::cout << "主线程正在忙其他事情" << std::endl;

    // 当需要结果时，调用 future 的 get() 方法来获取结果
    // 如果结果尚未准备好，get() 将阻塞直到结果可用
    int value = result.get();

    // 现在我们得到了异步任务的结果
    std::cout << "异步任务的结果是: " << value << std::endl;

    return 0;
}