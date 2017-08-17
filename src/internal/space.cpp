#include "internal/space.h"

#include "internal/allocation.h"
#include "internal/check.h"
#include "internal/object.h"

#include <atomic>
#include <new>


namespace worthy {
namespace internal {


namespace {


typedef std::atomic<std::uint32_t> AtomicRefCount;


} // namespace


#define STATIC_ASSERT_COMPATIBLE_MEMBER_TYPE(cls, member, type)                 \
    static_assert(sizeof(cls::member) >= sizeof(type),                          \
                  "size of " #cls "::" #member " too small for type " #type);   \
    static_assert(alignof(cls) >= alignof(type),                                \
                  "alignment of " #cls " too small for type " #type);           \
    static_assert(offsetof(cls, member) % alignof(type) == 0,                   \
                  "offset of " #cls "::" #member " misaligned for type " #type);


Space* Space::spaceOf(Object* obj) {
    return pageOf(obj)->space();
}


Page* Space::pageOf(Object* obj) {
    WORTHY_CHECK(obj);
    return Page::from(obj, obj->page_offset_);
}


Space::Space(Heap* heap) : heap_{heap} {
}


Space::~Space() {
    deletePages();
}


void Space::initialize(Object* obj, Page* page) {
    obj->page_offset_ = page->offsetOf(obj);
}


void Space::initRefCount(Object* obj) {
    STATIC_ASSERT_COMPATIBLE_MEMBER_TYPE(Object, control_, AtomicRefCount);

    WORTHY_CHECK(obj);

    // TODO: Set a ref-counted flag?

    new (&obj->control_) AtomicRefCount(1);
}


std::uint32_t Space::refCount(Object* obj) {
    WORTHY_CHECK(obj);

    AtomicRefCount* rc = reinterpret_cast<AtomicRefCount*>(&obj->control_);
    return rc->load(std::memory_order_relaxed);
}


void Space::retainRef(Object* obj) {
    WORTHY_CHECK(obj);

    AtomicRefCount* rc = reinterpret_cast<AtomicRefCount*>(&obj->control_);
    rc->fetch_add(1, std::memory_order_relaxed);
}


void Space::releaseRef(Object* obj) {
    WORTHY_CHECK(obj);

    AtomicRefCount* rc = reinterpret_cast<AtomicRefCount*>(&obj->control_);

    if (rc->fetch_sub(1, std::memory_order_release) == 1) {
        std::atomic_thread_fence(std::memory_order_acquire);
        reclaim(obj);
    }
}


void Space::reclaim(Object* obj) {
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

    void* memory = allocateAligned(page_size, Page::Alignment);
    if (!memory) {
        return nullptr;
    }

    return new (memory) Page(this, data_size);
}


void Space::deletePages() {
    std::lock_guard<std::mutex> lock(mutex_);

    while (!pages_.empty()) {
        pages_.pop_front_and_dispose(deallocateAligned);
    }
}


} } // namespace worthy::internal
