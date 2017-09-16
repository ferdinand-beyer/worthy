#include "internal/handle.h"

#include "internal/handle_container.h"


namespace worthy {
namespace internal {


Handle::Handle(Object* obj, ConstructKey)
    : obj_{obj}, ref_count_{0} {
}


Object* Handle::get() const {
    return obj_;
}


void Handle::retain() {
    ref_count_.fetch_add(1, std::memory_order_relaxed);
}


void Handle::release() {
    if (ref_count_.fetch_sub(1, std::memory_order_release) == 1) {
        std::atomic_thread_fence(std::memory_order_acquire);
        HandleAccess::reclaim(this);
    }
}


} } // namespace worthy::internal
