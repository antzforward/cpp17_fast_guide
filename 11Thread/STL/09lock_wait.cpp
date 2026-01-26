#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>

std::mutex g_display_mutex;
std::condition_variable cv;
bool ready = false;

void print_thread_id(int id) {
    std::unique_lock<std::mutex> lock(g_display_mutex);
    while (!ready) {  // 防止虚假唤醒
        cv.wait(lock);
    }
    std::cout << "thread " << id << " sleeping...\n";
}

void go() {
    std::unique_lock<std::mutex> lock(g_display_mutex);
    ready = true;
    cv.notify_all();
}

int main() {
    unsigned int n = std::thread::hardware_concurrency();
    std::cout << n << " concurrent threads are supported.\n";

    const unsigned int totalThreads = n;
    std::vector<std::thread> threads;

    for (unsigned int i = 0; i < totalThreads; ++i) {
        threads.emplace_back(print_thread_id, i);
    }

    // Sleep a little to ensure all threads are waiting
    std::this_thread::sleep_for(std::chrono::seconds(1));

    go(); // Notify all threads to proceed

    for (auto& t : threads) {
        t.join();
    }
}