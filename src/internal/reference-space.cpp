#include "internal/reference-space.h"

#include "internal/check.h"
#include "internal/reference.h"

#include <atomic>
#include <cstdlib>
#include <new>


namespace worthy {
namespace internal {


class ReferenceSpace::DeprPage {
public:
    inline static DeprPage* of(Reference* ref) {
        // By subtracting the index from the ref pointer, we get the start of
        // the data array, which is sizeof(DeprPage) away from the page start.
        return reinterpret_cast<DeprPage*>(ref - ref->pageIndex()) - 1;
    }

    DeprPage(ReferenceSpace* space, std::uint32_t capacity);

    inline Reference* data() {
        // (this + 1) will point directly after the page.
        return reinterpret_cast<Reference*>(this + 1);
    }

    Reference* allocate(void* ptr);

    // Number of Reference objects this page can hold.  We use 32-bit numbers
    // (instead of size_t) to save memory for the Reference's index field.
    const std::uint32_t capacity_;

    // Number of allocated objects.
    std::atomic<std::uint32_t> allocated_;

    ReferenceSpace* const space_;

    DeprPage* next_;
    DeprPage* prev_;
};


ReferenceSpace::DeprPage::DeprPage(ReferenceSpace* space, std::uint32_t capacity) :
    capacity_{capacity},
    allocated_{0},
    space_{space},
    next_{nullptr},
    prev_{nullptr}
{
    WORTHY_DCHECK(capacity > 0);
}


Reference* ReferenceSpace::DeprPage::allocate(void* ptr) {
    std::uint32_t index = allocated_.load(std::memory_order_relaxed);

    do {
        if (index == capacity_) {
            return nullptr;
        }
    } while (!allocated_.compare_exchange_weak(index, index + 1,
                                               std::memory_order_release,
                                               std::memory_order_relaxed));

    WORTHY_DCHECK(index < capacity_);

    void* memory = data() + index;

    return new (memory) Reference(index, ptr);
}


ReferenceSpace* ReferenceSpace::ownerOf(Reference* ref) {
    return DeprPage::of(ref)->space_;
}


ReferenceSpace::ReferenceSpace(Heap* heap, std::uint32_t page_capacity) :
    Space(heap),
    page_capacity_{page_capacity},
    top_page_{nullptr},
    free_list_{nullptr}
{
    WORTHY_CHECK(page_capacity_ > 0);
}


ReferenceSpace::~ReferenceSpace() {
    DeprPage* page = top_page_.exchange(nullptr, std::memory_order_acq_rel);

    while (page) {
        DeprPage* prev = page->prev_;
        std::free(page);
        page = prev;
    }
}


Reference* ReferenceSpace::newReference(void* ptr) {
    DeprPage* page = top_page_.load(std::memory_order_acquire);
    Reference* ref = page ? page->allocate(ptr) : nullptr;

    while (!ref) {
        ref = allocateFromFreeList(ptr);

        if (!ref) {
            page = allocatePageSync(page);
            if (!page) {
                return nullptr;
            }
            ref = page->allocate(ptr);
        }
    }

    WORTHY_DCHECK(ref->ptr() == ptr);

    return ref;
}


Reference* ReferenceSpace::allocateFromFreeList(void* ptr) {
    Reference* ref = free_list_.load(std::memory_order_relaxed);

    while (ref && !free_list_.compare_exchange_weak(
                ref, static_cast<Reference*>(ref->ptr()),
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


ReferenceSpace::DeprPage* ReferenceSpace::allocatePageSync(DeprPage* top_page) {
    std::lock_guard<std::mutex> lock(mutex_);

    // Load the top page again as another thread could have allocated a
    // new page in the meantime.
    DeprPage* page = top_page_.load(std::memory_order_acquire);

    if (page == top_page) {
        page = allocatePage();
        if (!page) {
            // Out of memory.
            return nullptr;
        }

        if (top_page) {
            page->prev_ = top_page;
            top_page->next_ = page;
        }

        top_page_.store(page, std::memory_order_release);
    }

    return page;
}


ReferenceSpace::DeprPage* ReferenceSpace::allocatePage() {
    // We use memory directly after the DeprPage structure for an array of
    // Reference objects.  Assert that this will be propertly aligned.
    static_assert((sizeof(DeprPage) % alignof(Reference)) == 0,
                  "Reference requires proper alignment");

    const std::size_t size = sizeof(DeprPage) + page_capacity_ * sizeof(Reference);

    void* memory = std::malloc(size);
    if (!memory) {
        return nullptr;
    }

    return new (memory) DeprPage(this, page_capacity_);
}


} } // namespace worthy::internal
