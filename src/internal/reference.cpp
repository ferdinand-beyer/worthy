#include "internal/reference.h"

#include "internal/check.h"
#include "internal/reference-space.h"


namespace worthy {
namespace internal {


Reference::Reference(std::uint32_t index, void* ptr) :
    ptr_{ptr},
    count_{1},
    index_{index}
{
}


Object* Reference::object() {
    WORTHY_CHECK(isValid());
    return static_cast<Object*>(ptr_);
}


void Reference::use() {
    WORTHY_CHECK(isValid());
    count_.fetch_add(1, std::memory_order_relaxed);
}


void Reference::release() {
    WORTHY_CHECK(isValid());
    if (count_.fetch_sub(1, std::memory_order_release) == 1) {
        std::atomic_thread_fence(std::memory_order_acquire);

        // Tell the space that this Reference can now be reused.
        ReferenceSpace::ownerOf(this)->addToFreeList(this);
    }
}


void Reference::reset(void* ptr) {
    // Called from ReferenceSpace when re-using.
    ptr_ = ptr;
    count_.store(1, std::memory_order_relaxed);
}


} } // namespace worthy::internal
