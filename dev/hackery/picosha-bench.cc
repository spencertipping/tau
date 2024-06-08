#include <iostream>
#include <chrono>
#include <vector>
#include <array>
#include "picosha2.h"

#define XXH_INLINE_ALL 1
#include "xxhash.h"

double measure_average_xxh64_hash_time(size_t num_trials = 10000000) {
    // Prepare a 64-byte buffer
    std::array<unsigned char, 64> buffer;
    for (int i = 0; i < 64; ++i) {
        buffer[i] = static_cast<unsigned char>(i);
    }

    // Prepare variables for holding the result and time measurements
    std::vector<std::chrono::duration<double, std::micro>> times(num_trials);
    XXH64_hash_t hash_result;
    uint64_t seed = 0; // Seed value for XXH64

    // Loop to perform the trials
    for (size_t i = 0; i < num_trials; ++i) {
        auto start_time = std::chrono::high_resolution_clock::now();

        // Perform the hash
        hash_result = XXH64(buffer.data(), buffer.size(), seed);

        auto end_time = std::chrono::high_resolution_clock::now();

        // Record the duration
        times[i] = end_time - start_time;
    }

    // Calculate average time
    double total_time = 0.0;
    for (size_t i = 0; i < num_trials; ++i) {
        total_time += times[i].count();
    }
    double average_time = total_time / num_trials;

    return average_time;
}

double measure_average_xxh128_hash_time(size_t num_trials = 10000000) {
    // Prepare a 64-byte buffer
    std::array<unsigned char, 64> buffer;
    for (int i = 0; i < 64; ++i) {
        buffer[i] = static_cast<unsigned char>(i);
    }

    // Prepare variables for holding the result and time measurements
    std::vector<std::chrono::duration<double, std::micro>> times(num_trials);
    XXH128_hash_t hash_result;
    uint64_t seed = 0; // Seed value for XXH64

    // Loop to perform the trials
    for (size_t i = 0; i < num_trials; ++i) {
        auto start_time = std::chrono::high_resolution_clock::now();

        // Perform the hash
        hash_result = XXH3_hashLong_128b_default(
            buffer.data(), buffer.size(), seed, nullptr, 0);

        auto end_time = std::chrono::high_resolution_clock::now();

        // Record the duration
        times[i] = end_time - start_time;
    }

    // Calculate average time
    double total_time = 0.0;
    for (size_t i = 0; i < num_trials; ++i) {
        total_time += times[i].count();
    }
    double average_time = total_time / num_trials;

    return average_time;
}

// Function to measure the average time required to hash a 64-byte buffer
double measure_average_sha_hash_time(size_t num_trials = 1000000) {
    // Prepare a 64-byte buffer
    std::array<unsigned char, 64> buffer;
    for (int i = 0; i < 64; ++i) {
        buffer[i] = static_cast<unsigned char>(i);
    }

    // Prepare variables for holding the result and time measurements
    std::vector<std::chrono::duration<double, std::micro>> times(num_trials);
    std::string hash_bin_str;

    // Loop to perform the trials
    for (size_t i = 0; i < num_trials; ++i) {
        auto start_time = std::chrono::high_resolution_clock::now();

        // Perform the hash
        picosha2::hash256(buffer.begin(), buffer.end(), hash_bin_str);

        auto end_time = std::chrono::high_resolution_clock::now();

        // Record the duration
        times[i] = end_time - start_time;
    }

    // Calculate average time
    double total_time = 0.0;
    for (size_t i = 0; i < num_trials; ++i) {
        total_time += times[i].count();
    }
    double average_time = total_time / num_trials;

    return average_time;
}

int main() {
    // Measure and print the average hash time for a 64-byte buffer
    double average_sha_time = measure_average_sha_hash_time();
    double average_xxh64_time = measure_average_xxh64_hash_time();
    double average_xxh128_time = measure_average_xxh64_hash_time();

    std::cout << "Average time for SHA-256 hashing: " << average_sha_time << " microseconds" << std::endl;
    std::cout << "Average time for XXH64 hashing: " << average_xxh64_time << " microseconds" << std::endl;
    std::cout << "Average time for XXH128 hashing: " << average_xxh128_time << " microseconds" << std::endl;

    return 0;
}
