#include "internal/reference-space.h"

#include "internal/check.h"
#include "internal/reference.h"

#include <atomic>
#include <cstdlib>
#include <new>

namespace worthy {
namespace internal {

class ReferenceSpace::Page {
public:
    inline static Page* of(Reference* ref) {
        // By subtracting the index from the ref pointer, we get the start of
        // the data array, which is sizeof(Page) away from the page start.
        return reinterpret_cast<Page*>(ref - ref->pageIndex()) - 1;
    }

    Page(ReferenceSpace* space, std::size_t capacity);

    inline ReferenceSpace* space() {
        return space_;
    }

    inline Reference* data() {
        // (this + 1) will point directly after the page.
        return reinterpret_cast<Reference*>(this + 1);
    }

    Reference* allocate(void* ptr);

private:
    // Number of Reference objects this page can hold.
    const std::size_t capacity_;

    ReferenceSpace* const space_;

    //Page* next_page_;
    //Page* previous_page_;

    // Next free index.
    std::atomic<std::uint32_t> next_;
};

ReferenceSpace::Page::Page(ReferenceSpace* space, std::size_t capacity) :
    capacity_{capacity},
    space_{space},
    //next_page_{nullptr},
    //previous_page_{nullptr},
    next_{0}
{
    WORTHY_DCHECK(capacity > 0);
}

Reference* ReferenceSpace::Page::allocate(void* ptr) {
    std::uint32_t index = next_.load(std::memory_order_relaxed);

    do {
        if (index == capacity_) {
            return nullptr;
        }
    } while (!next_.compare_exchange_weak(index, index + 1,
                                          std::memory_order_release,
                                          std::memory_order_relaxed));

    WORTHY_DCHECK(index < capacity_);

    void* memory = data() + index;

    return new (memory) Reference(index, ptr);
}

ReferenceSpace* ReferenceSpace::ownerOf(Reference* ref) {
    return Page::of(ref)->space();
}

ReferenceSpace::ReferenceSpace(Heap* heap, std::size_t pageCapacity) :
    Space(heap),
    page_capacity_{pageCapacity},
    root_{nullptr},
    free_list_{nullptr}
{
    WORTHY_CHECK(page_capacity_ > 0);
}

ReferenceSpace::~ReferenceSpace() {
    // TODO: Free all pages
    std::free(root_);
}

Reference* ReferenceSpace::newReference(void* ptr) {
    Page* page;
    if (root_) {
        page = root_;
    } else {
        page = allocatePage();
    }

    Reference* ref = page->allocate(ptr);

    if (!ref) {
        ref = allocateFromFreeList(ptr);
    }

    return ref;
}

Reference* ReferenceSpace::allocateFromFreeList(void* ptr) {
    Reference* ref = free_list_.load(std::memory_order_relaxed);

    while (ref && !free_list_.compare_exchange_weak(
                ref, reinterpret_cast<Reference*>(ref->ptr()),
                std::memory_order_release,
                std::memory_order_relaxed)) {
    }

    if (ref) {
        ref->reset(ptr);
    }

    return ref;
}

void ReferenceSpace::addToFreeList(Reference* ref) {
    ref->ptr_ = free_list_.load(std::memory_order_relaxed);

    while (!free_list_.compare_exchange_weak(
                reinterpret_cast<Reference*&>(ref->ptr_), ref,
                std::memory_order_release,
                std::memory_order_relaxed)) {
    }
}

ReferenceSpace::Page* ReferenceSpace::allocatePage() {
    // We use memory directly after the Page structure for an array of
    // Reference objects.  Assert that this will be propertly aligned.
    static_assert((sizeof(Page) % alignof(Reference)) == 0,
                  "Reference requires proper alignment");

    // TODO: Support more than one page!
    WORTHY_CHECK(!root_);

    const std::size_t size = sizeof(Page) + page_capacity_ * sizeof(Reference);

    void* memory = std::malloc(size);
    if (!memory) {
        return nullptr;
    }

    Page* page = new (memory) Page(this, page_capacity_);
    root_ = page;

    return page;
}

} } // namespace worty::internal
