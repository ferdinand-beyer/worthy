#include "catch.hpp"

#include <atomic>
#include <cstdlib>
#include <iostream>
#include <thread>

inline void* aligned_alloc(size_t size, size_t alignment) {
    void* result;
#ifdef _MSC_VER 
    result = _aligned_malloc(size, alignment);
#else 
    if (posix_memalign(&result, alignment, size)) {
        result = nullptr;
    }
#endif
    return result;
}

inline void aligned_free(void* ptr) {
#ifdef _MSC_VER 
    _aligned_free(ptr);
#else
    free(ptr);
#endif
}

TEST_CASE("Foobar", "sandbox") {
    const int page_size_bits = 19;
    const size_t page_size = 1 << page_size_bits;

    std::printf("page size:             %zd\n", page_size);
    std::printf("sizeof void*:          %zd\n", sizeof(void*));
    std::printf("sizeof size_t:         %zd\n", sizeof(std::size_t));
    std::printf("sizeof atomic_size_t:  %zd\n", sizeof(std::atomic_size_t));
    std::printf("alignof atomic_size_t: %zd\n", alignof(std::atomic_size_t));
    std::printf("sizeof thread::id:     %zd\n", sizeof(std::thread::id));
    std::printf("alignof thread::id:    %zd\n", alignof(std::thread::id));

    void* p1 = std::malloc(10);
    std::printf("default-aligned addr:   %p\n", p1);
    std::free(p1);
 
    void* p2 = aligned_alloc(page_size, page_size);
    std::printf("page-size aligned addr: %p\n", p2);
    aligned_free(p2);
}

class Tracer {
public:
    Tracer() {
        std::printf("Tracer constructed at:     %p\n", this);
    }

    virtual ~Tracer() {
        std::printf("Tracer destructed at:     %p\n", this);
    }
};

class Subtracer : public Tracer {
public:
    Subtracer() {
        std::printf("Subtracer constructed at:  %p\n", this);
    }

    ~Subtracer() {
        std::printf("Subtracer destructed at:  %p\n", this);
    }
};

TEST_CASE("Placement new", "[experiments]") {
    char buffer[64];

    std::printf("--------------------------------------------------\n");
    std::printf("sizeof Tracer:     %zd\n", sizeof(Tracer));
    std::printf("sizeof Subtracer:  %zd\n", sizeof(Subtracer));
    std::printf("--------------------------------------------------\n");

    Tracer* p = new (buffer) Subtracer();

    REQUIRE(reinterpret_cast<void*>(p) == reinterpret_cast<void*>(buffer));

    std::printf("--------------------------------------------------\n");
    p->~Tracer();
}
