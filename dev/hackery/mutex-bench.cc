#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <chrono>

const long long ITERATIONS = 100000000LL; // Number of iterations

int main() {
    std::shared_mutex smutex;

    // Measuring unique lock time
    auto start_unique = std::chrono::high_resolution_clock::now();
    for (long long i = 0; i < ITERATIONS; ++i) {
        std::unique_lock<std::shared_mutex> uniqueLock(smutex);
    }
    auto end_unique = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> unique_duration = end_unique - start_unique;
    double unique_lock_avg = unique_duration.count() / ITERATIONS * 1e9; // Convert seconds to nanoseconds

    // Measuring shared lock time
    auto start_shared = std::chrono::high_resolution_clock::now();
    for (long long i = 0; i < ITERATIONS; ++i) {
        std::shared_lock<std::shared_mutex> sharedLock(smutex);
    }
    auto end_shared = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> shared_duration = end_shared - start_shared;
    double shared_lock_avg = shared_duration.count() / ITERATIONS * 1e9; // Convert seconds to nanoseconds

    // Output the average durations
    std::cout << "Average time to claim unique lock: " << unique_lock_avg << " nanoseconds" << std::endl;
    std::cout << "Average time to claim shared lock: " << shared_lock_avg << " nanoseconds" << std::endl;

    return 0;
}
