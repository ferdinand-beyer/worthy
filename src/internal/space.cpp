#include "internal/space.h"

#include "internal/check.h"
#include "internal/object.h"
#include "internal/object-header.h"

#include <boost/align/aligned_alloc.hpp>

#include <new>


using boost::alignment::aligned_alloc;
using boost::alignment::aligned_free;


namespace worthy {
namespace internal {


Space::Space(Heap* heap) : heap_{heap} {
}


Space::~Space() {
    deletePages();
}


void* Space::placeObjectHeader(void* memory, std::size_t size,
                               Page* page, ObjectType type) {
    ObjectHeader* header = new (memory) ObjectHeader(size, type);
    page->setMarker(&header->page_marker_);
    return header + 1;
}


void Space::reclaim(Object* obj) {
    // TODO: obj is no longer a root.
}


Page* Space::addPage(std::size_t data_size) {
    std::lock_guard<std::mutex> lock(mutex_);

    Page* page = allocatePage(data_size);

    if (!page) {
        // TODO: Notify runtime?
        return nullptr;
    }

    pages_.push_front(*page);

    return page;
}


Page* Space::allocatePage(std::size_t data_size) {
    const std::size_t page_size = sizeof(Page) + data_size;

    // TODO: Define MaxPageSize in terms of data_size?
    // For this, we must update offset computations to subtract
    // the page header.
    WORTHY_CHECK(page_size <= Page::MaxPageSize);

    void* memory = aligned_alloc(Page::Alignment, page_size);
    if (!memory) {
        return nullptr;
    }

    return new (memory) Page(this, data_size);
}


void Space::deletePages() {
    while (!pages_.empty()) {
        pages_.pop_front_and_dispose(aligned_free);
    }
}


} } // namespace worthy::internal
