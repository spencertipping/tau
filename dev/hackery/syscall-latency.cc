#include <iostream>
#include <chrono>
#include <unistd.h> // for getpid()

int main() {
    constexpr int iterations = 1000000;
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < iterations; ++i) {
        // Call getpid() system call
        (void) getpid();
    }

    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> duration = end - start;
    double average_ns = (duration.count() * 1e9) / iterations;

    std::cout << "Average latency of getpid() system call: "
              << average_ns
              << " nanoseconds\n";

    return 0;
}
