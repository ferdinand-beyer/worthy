#ifndef WORTHY_INTERNAL_PAGE_H_
#define WORTHY_INTERNAL_PAGE_H_


#include "internal/globals.h"
#include "internal/macros.h"

#include <boost/intrusive/list.hpp>

#include <atomic>
#include <cstddef>
#include <cstdint>


namespace worthy {
namespace internal {


class Space;


class Page : public boost::intrusive::list_base_hook<> {
public:
    // Alignment in bytes.  Together with the size of the PageMarker, this
    // determines the maximum page size.
    static const std::size_t Alignment = 16;

    // Number of bits for page markers in Objects.
    static const std::size_t MarkerBits = 8 * sizeof(PageMarker);

    // Maximum page size in bytes, including the header.
    static const std::size_t MaxPageSize = (1 << MarkerBits) * Alignment;

    // Return the Page from a page marker.  The marker must have been set
    // with setMarker() before.
    static Page* fromMarker(const PageMarker* marker);

    Page(Space* space, std::size_t data_size);

    Space* space();

    // Store a value in the given PageMarker that can later be used to
    // retrieve this page.  The marker must point to an address within
    // this page.
    void setMarker(PageMarker* marker) const;

    Address allocate(std::size_t size, std::size_t alignment);

protected:
    ~Page() = default;

private:
    WORTHY_DISABLE_COPY(Page);

    Address address() const;

    Address begin() const;
    Address end() const;

    // Tell if the given memory address lies within the page.
    bool contains(Address addr) const;

    Space* const space_;
    const std::size_t data_size_;
    std::atomic<Address> top_;
};


inline Space* Page::space() {
    return space_;
}


inline Address Page::address() const
{
    return reinterpret_cast<Address>(const_cast<Page*>(this));
}


inline Address Page::begin() const {
    // Return address directly after the page header.
    return reinterpret_cast<Address>(const_cast<Page*>(this + 1));
}


inline Address Page::end() const {
    return begin() + data_size_;
}


inline bool Page::contains(Address addr) const {
    return (addr >= begin()) && (addr < end());
}


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_PAGE_H_
