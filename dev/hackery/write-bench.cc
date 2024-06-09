#include <iostream>
#include <fstream>
#include <chrono>
#include <unistd.h>  // For close()
#include <fcntl.h>   // For open() and O_* constants
#include <cstring>   // For memset, for buffer initialization
#include <sys/mman.h> // For mmap

constexpr size_t TOTAL_DATA_SIZE = 64 * 1024 * 1024; // 64MB

void measure_write_time(const char* filename, size_t write_size) {
    size_t num_writes = TOTAL_DATA_SIZE / write_size;
    // Create and open a new file with write-only access
    int fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        std::cerr << "Failed to open file " << filename << std::endl;
        return;
    }

    // mmap the file into a buffer
    char *buf = (char *)mmap(NULL, TOTAL_DATA_SIZE, PROT_READ, MAP_SHARED, fd, 0);
    if (buf == MAP_FAILED) {
        std::cerr << "Failed to mmap file " << filename << std::endl;
        close(fd);
        return;
    }

    // Data to write
    char* buffer = new char[write_size];
    memset(buffer, 0, write_size); // Initialize buffer to zero

    // Get the current time before the write operations start
    auto start = std::chrono::high_resolution_clock::now();

    // Perform the write operations
    for (size_t i = 0; i < num_writes; ++i) {
        if (write(fd, buffer, write_size) != write_size) {
            std::cerr << "Error writing to file " << filename << std::endl;
            close(fd);
            delete[] buffer;
            return;
        }
    }

    // Get the current time after the write operations end
    auto end = std::chrono::high_resolution_clock::now();
    // Calculate the duration
    std::chrono::duration<double> duration = end - start;

    // Clean up
    close(fd);
    delete[] buffer;

    std::cout << "Time taken for writing 64MB with " << write_size
              << "-byte writes: " << duration.count() << " seconds = "
              << duration.count() / num_writes * 1e9 << " nanos/call"
              << std::endl;
}

int main() {
    measure_write_time("output_8_byte.bin", 8);
    measure_write_time("output_64_byte.bin", 64);
    return 0;
}
