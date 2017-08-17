#include "internal/space.h"

#include "internal/allocation.h"
#include "internal/check.h"
#include "internal/object.h"

#include <atomic>
#include <new>


namespace worthy {
namespace internal {


typedef std::atomic<std::uint32_t> AtomicRefCount;


#define STATIC_ASSERT_COMPATIBLE_MEMBER_TYPE(cls, member, type)                 \
    static_assert(sizeof(cls::member) >= sizeof(type),                          \
                  "size of " #cls "::" #member " too small for type " #type);   \
    static_assert(alignof(cls) >= alignof(type),                                \
                  "alignment of " #cls " too small for type " #type);           \
    static_assert(offsetof(cls, member) % alignof(type) == 0,                   \
                  "offset of " #cls "::" #member " misaligned for type " #type);


Space::Space(Heap* heap) : heap_{heap} {
}


Space::~Space() {
}


void Space::initRefCount(Object* obj) {
    STATIC_ASSERT_COMPATIBLE_MEMBER_TYPE(Object, control_, AtomicRefCount);

    WORTHY_CHECK(obj);

    // TODO: Set a ref-counted flag?

    new (&obj->control_) AtomicRefCount(1);
}


void Space::retainRef(Object* obj) {
    WORTHY_CHECK(obj);

    AtomicRefCount* rc = reinterpret_cast<AtomicRefCount*>(obj->control_);
    rc->fetch_add(1, std::memory_order_relaxed);
}


void Space::releaseRef(Object* obj) {
    WORTHY_CHECK(obj);

    AtomicRefCount* rc = reinterpret_cast<AtomicRefCount*>(obj->control_);

    if (rc->fetch_sub(1, std::memory_order_release) == 1) {
        std::atomic_thread_fence(std::memory_order_acquire);
        reclaim(obj);
    }
}


void Space::reclaim(Object* obj) {
}


void Space::deletePages() {
    std::lock_guard<std::mutex> lock(mutex_);

    while (!pages_.empty()) {
        pages_.pop_front_and_dispose(deallocateAligned);
    }
}


} } // namespace worthy::internal
