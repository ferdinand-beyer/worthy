#include "internal/reference-space.h"

#include "internal/check.h"
#include "internal/reference.h"

#include <atomic>
#include <cstdlib>
#include <new>

namespace worthy {
namespace internal {

struct ReferenceSpace::Page {
    const std::size_t capacity_;

    ReferenceSpace* space_;

    Page* next_page_;
    Page* previous_page_;

    std::atomic<std::uint32_t> next_;

    Page(ReferenceSpace* space, std::size_t capacity);

    Reference* data();

    Reference* allocate(void* ptr);
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

inline Reference* ReferenceSpace::Page::data() {
    return reinterpret_cast<Reference*>(this + 1);
}

Reference* ReferenceSpace::Page::allocate(void* ptr) {
    std::uint32_t index;
    
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

    return ref;
}

ReferenceSpace::Page* ReferenceSpace::allocatePage(std::size_t capacity) {
    // TODO: Support more than one page!
    WORTHY_DCHECK(!root_);

    const std::size_t size = sizeof(ReferenceSpace::Page) +
                             sizeof(Reference) * capacity;

    void* memory = std::malloc(size);
    if (!memory) {
        return nullptr;
    }

    Page* page = new (memory) Page(this, capacity);
    root_ = page;

    return page;
}

} } // namespace worty::internal
