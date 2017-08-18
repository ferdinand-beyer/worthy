#include "internal/page.h"

#include "internal/check.h"
#include "internal/memory.h"


namespace worthy {
namespace internal {


Page* Page::fromMarker(const PageMarker* marker) {
    WORTHY_CHECK(marker);
    const Address markerAddress = reinterpret_cast<Address>(
            const_cast<PageMarker*>(marker));
    const Address pageAddress = roundDown<Alignment>(markerAddress)
        - *marker * Alignment;
    return reinterpret_cast<Page*>(pageAddress);
}


Page::Page(Space* space, std::size_t data_size)
    : space_{space},
      data_size_{data_size},
      top_{begin()} {
    // TODO: Check for MaxPageSize.  Or, if we want to allow
    // larger pages for huge objects, make sure that we don't
    // allocate objects after the maximum marker point.
}


void Page::setMarker(PageMarker* marker) const {
    const Address markerAddress = reinterpret_cast<Address>(marker);
    WORTHY_CHECK(contains(markerAddress));
    *marker = (roundDown<Alignment>(markerAddress) - address()) / Alignment;
}


Address Page::allocate(std::size_t size, std::size_t alignment) {
    WORTHY_CHECK(size > 0);

    Address top = top_.load(std::memory_order_relaxed);
    Address result;
    Address new_top;

    do {
        result = roundUp(top, alignment);
        new_top = result + size;

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
