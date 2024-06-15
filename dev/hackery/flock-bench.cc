#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>
#include <chrono>

void benchmarkSharedLock(const char* filename, int iterations) {
    // Open the file descriptor
    int fd = open(filename, O_RDWR);
    if (fd == -1) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    // Define the lock structure
    struct flock lock;
    lock.l_type = F_RDLCK; // Shared lock
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0; // 0 means to lock the whole file

    // Total duration for all iterations
    std::chrono::duration<double> totalDuration(0);

    // Repeat lock acquisition and release for the given number of iterations
    for (int i = 0; i < iterations; ++i) {
        // Start timing
        auto start = std::chrono::high_resolution_clock::now();

        // Try to acquire the lock
        if (fcntl(fd, F_SETLK, &lock) == -1) {
            perror("Failed to acquire lock");
            close(fd);
            exit(EXIT_FAILURE);
        }

        // Stop timing
        auto end = std::chrono::high_resolution_clock::now();

        // Calculate the elapsed time for this iteration
        std::chrono::duration<double> elapsedSeconds = end - start;
        totalDuration += elapsedSeconds;

        // Release the lock
        lock.l_type = F_UNLCK;
        if (fcntl(fd, F_SETLK, &lock) == -1) {
            perror("Failed to release lock");
            close(fd);
            exit(EXIT_FAILURE);
        }
    }

    // Calculate and print average lock acquisition time
    double averageTime = totalDuration.count() / iterations;
    std::cout << "Average time to acquire shared lock: " << averageTime * 1e9 << " ns\n";

    // Clean up
    close(fd);
}

int main() {
    const char* filename = "testfile.txt";
    const int iterations = 1000000;

    // Create a dummy file with some content
    std::ofstream outfile(filename);
    if (!outfile) {
        std::cerr << "Failed to create file" << std::endl;
        return EXIT_FAILURE;
    }
    outfile << "This is a test file." << std::endl;
    outfile.close();

    // Benchmark the shared lock acquisition
    benchmarkSharedLock(filename, iterations);

    // Clean up the created file
    if (remove(filename) != 0) {
        perror("Failed to remove file");
    }

    return 0;
}
