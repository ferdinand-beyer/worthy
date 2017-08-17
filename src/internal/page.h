#ifndef WORTHY_INTERNAL_PAGE_H_
#define WORTHY_INTERNAL_PAGE_H_


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
    // TODO: Use the same type for Object::page_offset_
    typedef std::uint16_t Offset;

    static const std::size_t OffsetBits = 8 * sizeof(Offset);

    // Size of a logical block in bytes.
    static const std::size_t BlockSize = 16;

    // The page must be aligned to the block size to support allocations
    // smaller than the block size.
    static const std::size_t Alignment = BlockSize;

    // Maximum number of addressable blocks.
    static const std::size_t MaxBlockCount = 1 << OffsetBits;

    // Maximum page size in bytes.
    static const std::size_t MaxPageSize = MaxBlockCount * BlockSize;

    static Page* from(void* addr, Offset offset);

    Page(Space* space, std::size_t data_size);

    Space* space();

    Offset offsetOf(void* addr) const;

    void* allocate(std::size_t size, std::size_t alignment);

protected:
    ~Page() = default;

private:
    WORTHY_DISABLE_COPY(Page);

    void* begin();
    void* end();

    Space* const space_;
    const std::size_t data_size_;
    std::atomic<void*> top_;
};


inline Space* Page::space() {
    return space_;
}


inline void* Page::begin() {
    // Return address directly after the page header.
    return (this + 1);
}


inline void* Page::end() {
    return reinterpret_cast<char*>(begin()) + data_size_;
}


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_PAGE_H_
