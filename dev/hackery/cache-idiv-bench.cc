#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>
#include <random>
#include <iomanip>

// Function to get the current time in nanoseconds
inline uint64_t current_time_ns() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

// Measure the access time for array elements
double measure_cache(int bits, double baseline = 0) {
    size_t const iterations = 32ull * 1048576;
    size_t const test_size = 1ull << bits;
    std::vector<uint8_t> arr(test_size);

    uint64_t start_time = current_time_ns();
    size_t i = current_time_ns();
    if (bits == 0)
        for (size_t j = 0; j < iterations; ++j)
            i += (i *= 0x1957'8c12'5577'0413) & ~(-1ull << bits - 6) << 6;
    else
        for (size_t j = 0; j < iterations; ++j)
            i += arr[(i *= 0x1957'8c12'5577'0413) & ~(-1ull << bits - 6) << 6]++;
    uint64_t end_time = current_time_ns();
    double elapsed_time = static_cast<double>(end_time - start_time) - baseline;
    std::cout << std::fixed << std::setprecision(3)
              << i << ", " << test_size << ", "
              << elapsed_time / iterations << std::endl;
    return elapsed_time;
}

// Measure the latency of integer division
void measure_idiv_performance() {
    const size_t iterations = 10'000'000;
    size_t numerator = 123456789;
    size_t denominator = 12345;
    size_t result = 0;

    auto start_time = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterations; ++i) {
        result += numerator / denominator;
        if (!++denominator) denominator = 1;
        numerator += 3;
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::nano> elapsed_ns = end_time - start_time;
    double avg_div_time_ns = elapsed_ns.count() / iterations;

    std::cout << result << "; average integer division time: " << avg_div_time_ns << " ns" << std::endl;
}

int main() {
    measure_idiv_performance();

    auto b = measure_cache(0);
    for (int i = 10; i < 33; ++i)
        measure_cache(i, b);

    return 0;
}
