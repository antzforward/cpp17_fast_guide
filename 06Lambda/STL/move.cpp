#include <algorithm>
#include <chrono>
#include <iostream>
#include <iterator>
#include <list>
#include <thread>
#include <vector>
/*移动范围 [first, last) 中的元素到始于 d_first 的另一范围，从 first 开始逐次到 last - 1。此操作后被移动范围中的元素将仍然含有适合类型的合法值，但不必与移动前的值相同。*/ 
void f(int n)
{
    std::this_thread::sleep_for(std::chrono::seconds(n));
    std::cout << "线程 " << n << " 终止" << std::endl;
}
/*移动重叠的范围时，std::move 在向左侧移动（目标范围的起始在源范围外）时适合，而 std::move_backward 在向右侧移动（目标范围的结尾在源范围外）时适合。*/ 
int main() 
{
    std::vector<std::thread > v;// 使用 std::thread 而不是 std::jthread
    v.emplace_back(f, 1);
    v.emplace_back(f, 2);
    v.emplace_back(f, 3);
    std::list<std::thread > l;
 
    // copy() 无法编译，因为 std::jthread 不可复制
    std::move(v.begin(), v.end(), std::back_inserter(l));
    
    // 确保移动后原向量中的每个元素都没有关联的线程
    v.clear();

    // 加入所有线程以确保它们完成
    for (auto& t : l) {
        if (t.joinable()) t.join();
    }

    return 0;
}