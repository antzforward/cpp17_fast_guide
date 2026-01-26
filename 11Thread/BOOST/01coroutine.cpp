#include <boost/coroutine2/all.hpp>
#include <iostream>

// Define the types for coroutines
typedef boost::coroutines2::asymmetric_coroutine<int> coro_t;

// The coroutine function, which is the producer of integers
void countingCoroutine(coro_t::push_type& sink) {
    int count = 0;
    while (true) {
        sink(count++); // Produce a value to the consumer
    }
}

int main() {
    // Create the coroutine and bind it to the countingCoroutine function
    coro_t::pull_type source(countingCoroutine);
    
    // Consume the first 10 values produced by the coroutine
    for (int i = 0; i < 10; ++i) {
        std::cout << source.get() << std::endl; // Get the current value
        source(); // Resume the coroutine to get the next value
    }

    return 0;
}
//编译过程要附件link的库 比如 cl /EHsc 01coroutine.cpp boost_context-vc140-mt.lib 当然写个requestment文件也可以