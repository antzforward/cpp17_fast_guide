#include <iostream>
#include <atomic>
#include <thread>

struct Node {
    int value;
    std::atomic<Node*> next;
    Node(int val) : value(val), next(nullptr) {}
};

std::atomic<Node*> head(nullptr);
std::atomic<Node*> tail(nullptr);

void push_back(int value) {
    Node* newNode = new Node(value);
    Node* oldTail = tail.load(std::memory_order_acquire);

    while (true) {
        Node* last = tail.load(std::memory_order_acquire);
        Node* next = last ? last->next.load(std::memory_order_acquire) : nullptr;

        if (last == tail.load(std::memory_order_acquire)) {
            if (next == nullptr) {
                // Try to link the new node at the end of the list
                if (last->next.compare_exchange_strong(next, newNode, std::memory_order_release, std::memory_order_relaxed)) {
                    // Update the tail to point to the new node
                    tail.compare_exchange_strong(last, newNode, std::memory_order_release, std::memory_order_relaxed);
                    return;
                }
            }
            else {
                // Help update the tail
                tail.compare_exchange_strong(last, next, std::memory_order_release, std::memory_order_relaxed);
            }
        }
    }
}

int pop_front() {
    while (true) {
        Node* oldHead = head.load(std::memory_order_acquire);
        if (!oldHead) {
            return -1; // Queue is empty
        }

        Node* nextNode = oldHead->next.load(std::memory_order_acquire);
        if (head.compare_exchange_strong(oldHead, nextNode, std::memory_order_release, std::memory_order_relaxed)) {
            int value = oldHead->value;
            delete oldHead;  // Release memory
            return value;
        }
    }
}

void test() {
    const int iterations = 10;

    for (int i = 0; i < iterations; ++i) {
        auto t = std::thread([=]() { push_back(i); });
        t.join();
    }

    int sum = 0;
    for (int i = 0; i < iterations; ++i) {
        int value = pop_front();
        if (value != -1) {
            sum += value;
        }
    }

    std::cout << "Sum: " << sum << std::endl;
}

int main() {
    std::thread t1(push_back, 1);
    std::thread t2(push_back, 2);

    t1.join();
    t2.join();

    std::cout << "Popped: " << pop_front() << std::endl;
    std::cout << "Popped: " << pop_front() << std::endl;

    std::cout << "Popped: " << pop_front() << std::endl;
    std::cout << "Popped: " << pop_front() << std::endl;

    //test();

    return 0;
}