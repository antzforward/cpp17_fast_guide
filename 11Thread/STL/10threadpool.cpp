// 注意: 以下代码需要额外的线程池库支持
#include <iostream>
#include <vector>
#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>

void print_id(int id) {
    std::cout << "thread " << id << " working...\n";
}

int main() {
    boost::asio::thread_pool pool(std::thread::hardware_concurrency());

    for (int i = 0; i < 10; ++i) {
        boost::asio::post(pool, std::bind(print_id, i));
    }

    pool.join();
}