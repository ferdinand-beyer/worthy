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

    Page* next_page_;
    Page* previous_page_;

    // Next free index.
    std::atomic<std::uint32_t> next_;
};

ReferenceSpace::Page::Page(ReferenceSpace* space, std::size_t capacity) :
    capacity_{capacity},
    space_{space},
    next_page_{nullptr},
    previous_page_{nullptr},
    next_{0}
{
    WORTHY_DCHECK(capacity > 0);
}

Reference* ReferenceSpace::Page::allocate(void* ptr) {
    std::uint32_t index;

    // Atomically get the next free index.
    do {
        index = next_.load(std::memory_order_relaxed);

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

ReferenceSpace::ReferenceSpace(Heap* heap) :
    Space(heap),
    root_{nullptr}
{
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

    // TODO: If ref is a nullptr, need to look into the free list
    // or allocate a new page.
    WORTHY_CHECK(ref);

    return ref;
}

ReferenceSpace::Page* ReferenceSpace::allocatePage(std::size_t capacity) {
    // We use memory directly after the Page structure for an array of
    // Reference objects.  Assert that this will be propertly aligned.
    static_assert((sizeof(Page) % alignof(Reference)) == 0,
                  "Reference requires proper alignment");

    // TODO: Support more than one page!
    WORTHY_CHECK(!root_);

    const std::size_t size = sizeof(Page) + capacity * sizeof(Reference);

    void* memory = std::malloc(size);
    if (!memory) {
        return nullptr;
    }

    Page* page = new (memory) Page(this, capacity);
    root_ = page;

    return page;
}

} } // namespace worty::internal
