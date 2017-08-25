#include "internal/page.h"

#include "internal/align.h"
#include "internal/check.h"


namespace worthy {
namespace internal {


Page* Page::fromMarker(const PageMarker* marker) {
    WORTHY_CHECK(marker);

    return reinterpret_cast<Page*>(
            alignDown(addressOf(marker), Alignment)
            - *marker * Alignment);
}


Page::Page(Space* space, size_t data_size)
    : space_{space},
      data_size_{data_size},
      top_{begin()} {
    // TODO: Check for MaxPageSize.  Or, if we want to allow
    // larger pages for huge objects, make sure that we don't
    // allocate objects after the maximum marker point.
}


void Page::setMarker(PageMarker* marker) const {
    WORTHY_CHECK(contains(marker));

    *marker = (alignDown(addressOf(marker), Alignment)
                - addressOf(this)) / Alignment;
}


void* Page::allocate(size_t size, size_t alignment) {
    WORTHY_CHECK(size > 0);

    uintptr_t top = top_.load(std::memory_order_relaxed);
    uintptr_t result;
    uintptr_t new_top;

    do {
        result = alignUp(top, alignment);
        new_top = result + size;

        if (new_top > end()) {
            // Not enough space.
            return nullptr;
        }
    } while (!top_.compare_exchange_weak(top, new_top,
                                         std::memory_order_release,
                                         std::memory_order_relaxed));

    return reinterpret_cast<void*>(result);
}


bool Page::contains(void* ptr) const {
    const auto addr = addressOf(ptr);
    return (addr >= begin()) && (addr < end());
}


} } // namespace worthy::internal
