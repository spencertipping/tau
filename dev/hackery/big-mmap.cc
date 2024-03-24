#include <iostream>
#include <sys/mman.h>
#include <cstring> // For strerror
#include <unistd.h> // For sysconf

int main() {
    size_t size = sysconf(_SC_PAGESIZE); // Start with the size of one page
    void* addr = nullptr;

    while (true) {
        std::cout << "Attempting to mmap " << size << " bytes (" << size / (1024.0 * 1024.0 * 1024.0) << " GiB)..." << std::endl;

        // Attempt to mmap
        addr = mmap(nullptr, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        
        // Check if mmap was successful
        if (addr == MAP_FAILED) {
            std::cerr << "mmap failed: " << strerror(errno) << std::endl;
            break; // Exit the loop if mmap fails
        } else {
            std::cout << "mmap successful!" << std::endl;
            // Clean up the current mapping before trying the next size
            munmap(addr, size);
            size *= 2; // Double the size for the next iteration
        }
    }

    return 0;
}
