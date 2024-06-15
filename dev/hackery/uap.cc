#include <type_traits>
#include <cstring>

// Forward declaration
template <typename T>
class UnalignedPointer;

// Partial specialization for const type
template <typename T>
class UnalignedPointer<const T> {
public:
    explicit UnalignedPointer(const void* p) : ptr(p) {}

    T load() const {
        T value;
        std::memcpy(&value, ptr, sizeof(T));
        return value;
    }

private:
    const void* ptr;
};

// Partial specialization for non-const type
template <typename T>
class UnalignedPointer {
public:
    explicit UnalignedPointer(void* p) : ptr(p) {}
    explicit UnalignedPointer(const void* p) : ptr(p) {}

    T load() const {
        T value;
        std::memcpy(&value, ptr, sizeof(T));
        return value;
    }

    void store(const T& value) {
        std::memcpy(ptr, &value, sizeof(T));
    }

private:
    void* ptr;
};

// Type inference function templates
template <typename T>
UnalignedPointer<std::remove_const_t<T>> MakeUnalignedPointer(T* p) {
    return UnalignedPointer<std::remove_const_t<T>>(p);
}

template <typename T>
UnalignedPointer<const T> MakeUnalignedPointer(const T* p) {
    return UnalignedPointer<const T>(p);
}

int main() {
    int value = 42;
    void* p = &value;
    const void* cp = &value;

    auto up = MakeUnalignedPointer(static_cast<int*>(p));        // Non-const instance
    auto cup = MakeUnalignedPointer(static_cast<const int*>(cp));// Const instance

    int loadedValue = up.load();
    int constLoadedValue = cup.load();

    up.store(100);  // Store a new value via non-const instance

    return 0;
}
