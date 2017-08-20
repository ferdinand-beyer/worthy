#include "internal/reference-space.h"

#include "internal/check.h"
#include "internal/objects.h"

#include <new>


namespace worthy {
namespace internal {


ReferenceSpace::ReferenceSpace(Heap* heap, std::uint32_t page_capacity) :
    Space(heap),
    page_capacity_{page_capacity},
    free_list_{nullptr}
{
    WORTHY_CHECK(page_capacity_ > 0);
}


ReferenceSpace::~ReferenceSpace() {
    // TODO: Check that all refcounts are zero?
}


Reference* ReferenceSpace::newReference(Object* obj) {
    WORTHY_CHECK(obj);

    Page* page = firstPage();

    Reference* ref = page ? allocateFromPage(page, obj) : nullptr;

    while (!ref) {
        ref = allocateFromFreeList(obj);

        if (!ref) {
            page = addPage();
            if (!page) {
                return nullptr;
            }
            ref = allocateFromPage(page, obj);
        }
    }

    WORTHY_DCHECK(ref->get() == obj);

    return ref;
}


std::uint32_t ReferenceSpace::refCount(Reference* ref) {
    return Space::refCount(ref);
}


void ReferenceSpace::retain(Reference* ref) {
    retainRef(ref);
}


void ReferenceSpace::release(Reference* ref) {
    releaseRef(ref);
}


Reference* ReferenceSpace::allocateFromPage(Page* page, Object* obj) {
    Address memory = page->allocate(sizeof(Reference), alignof(Reference));
    if (!memory) {
        return nullptr;
    }

    Reference* ref = new (memory) Reference(obj);

    initialize(ref, page);
    initRefCount(ref);

    return ref;
}


Reference* ReferenceSpace::allocateFromFreeList(Object* obj) {
    Reference* ref = free_list_.load(std::memory_order_relaxed);

    while (ref && !free_list_.compare_exchange_weak(
                ref, static_cast<Reference*>(ref->get()),
                std::memory_order_release,
                std::memory_order_relaxed)) {
    }

    if (ref) {
        WORTHY_DCHECK(refCount(ref) == 0);
        ref->ptr_ = obj;
    }

    return ref;
}


void ReferenceSpace::reclaim(Object* obj) {
    Reference* ref = Reference::cast(obj);

    ref->ptr_ = free_list_.load(std::memory_order_relaxed);

    while (!free_list_.compare_exchange_weak(
                reinterpret_cast<Reference*&>(ref->ptr_), ref,
                std::memory_order_release,
                std::memory_order_relaxed)) {
    }
}


Page* ReferenceSpace::addPage() {
    // TODO: Synchronize!
    // Pass current page to Space::addPage?

    // TODO: Check if we need more space for alignment?

    return Space::addPage(page_capacity_ * sizeof(Reference));
}


} } // namespace worthy::internal
