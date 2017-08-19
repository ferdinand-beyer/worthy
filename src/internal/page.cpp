#include "internal/page.h"

#include "internal/check.h"

#include <boost/align/align_down.hpp>
#include <boost/align/align_up.hpp>


using boost::alignment::align_down;
using boost::alignment::align_up;


namespace worthy {
namespace internal {


Page* Page::fromMarker(const PageMarker* marker) {
    WORTHY_CHECK(marker);

    return reinterpret_cast<Page*>(
        static_cast<Address>(align_down(const_cast<PageMarker*>(marker),
                                        Alignment))
        - *marker * Alignment);
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

    *marker = (static_cast<Address>(align_down(markerAddress, Alignment))
                    - address()) / Alignment;
}


Address Page::allocate(std::size_t size, std::size_t alignment) {
    WORTHY_CHECK(size > 0);

    Address top = top_.load(std::memory_order_relaxed);
    Address result;
    Address new_top;

    do {
        result = static_cast<Address>(align_up(top, alignment));
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
