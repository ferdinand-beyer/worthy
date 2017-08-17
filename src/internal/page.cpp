#include "internal/page.h"

#include "internal/check.h"
#include "internal/memory.h"


namespace worthy {
namespace internal {


Page* Page::from(void* addr, Offset offset) {
    return reinterpret_cast<Page*>(roundDown<Alignment>(static_cast<char*>(addr))
            - offset * BlockSize);
}


Page::Page(Space* space, std::size_t data_size)
    : space_{space},
      data_size_{data_size},
      top_{begin()} {
}


Page::Offset Page::offsetOf(void* addr) const {
    return (roundDown<Alignment>(static_cast<char*>(addr))
                - reinterpret_cast<const char*>(this)) / BlockSize;
}


void* Page::allocate(std::size_t size, std::size_t alignment) {
    WORTHY_CHECK(size > 0);

    void* top = top_.load(std::memory_order_relaxed);
    void* result;
    void* new_top;

    do {
        result = roundUp(reinterpret_cast<char*>(top), alignment);
        new_top = static_cast<char*>(result) + size;

        if (new_top > end()) {
            // Not enough space.
            return nullptr;
        }
    } while (!top_.compare_exchange_weak(top, new_top,
                                         std::memory_order_release,
                                         std::memory_order_relaxed));

    return result;
}


} } // namespace worthy::internal
