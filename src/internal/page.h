#ifndef WORTHY_INTERNAL_PAGE_H_
#define WORTHY_INTERNAL_PAGE_H_


#include "internal/globals.h"

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
    static const size_t Alignment = 16;

    // Number of bits for page markers in Objects.
    static const size_t MarkerBits = 8 * sizeof(PageMarker);

    // Maximum page size in bytes, including the header.
    static const size_t MaxPageSize = (1 << MarkerBits) * Alignment;

    // Return the Page from a page marker.  The marker must have been set
    // with setMarker() before.
    static Page* fromMarker(const PageMarker* marker);

    Page(Space* space, size_t data_size);

    Space* space();

    // Store a value in the given PageMarker that can later be used to
    // retrieve this page.  The marker must point to an address within
    // this page.
    void setMarker(PageMarker* marker) const;

    void* allocate(size_t size, size_t alignment);

protected:
    ~Page() = default;

private:
    WORTHY_DISABLE_COPY(Page);

    uintptr_t begin() const;
    uintptr_t end() const;

    // Tell if the given memory address lies within the page.
    bool contains(void* ptr) const;

    Space* const space_;
    const size_t data_size_;
    std::atomic<uintptr_t> top_;
};


inline Space* Page::space() {
    return space_;
}


inline uintptr_t Page::begin() const {
    // Return address directly after the page header.
    return addressOf(this + 1);
}


inline uintptr_t Page::end() const {
    return begin() + data_size_;
}


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_PAGE_H_
