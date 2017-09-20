#include "internal/frame.h"

#include "internal/check.h"


namespace worthy {
namespace internal {


thread_local Frame* Frame::current_ = nullptr;


Frame& Frame::current() {
    WORTHY_DCHECK(current_ && current_->isLocked());
    return *current_;
}


Frame::Frame(size_t index, Heap* heap, BlockAllocator* allocator, ConstructKey)
    : index_{index},
      heap_{heap},
      allocator_{allocator},
      nursery_{heap, allocator} {
}


Heap& Frame::heap() {
    return *heap_;
}


Nursery& Frame::nursery() {
    return nursery_;
}


bool Frame::isLocked() const {
    return thread_id_ != std::thread::id();
}


bool Frame::tryLock() {
    WORTHY_DCHECK((current_ != this) || !isLocked());
    if (locked_.test_and_set(std::memory_order_acquire)) {
        // Was locked before.
        return false;
    }
    thread_id_ = std::this_thread::get_id();
    current_ = this;
    return true;
}


void Frame::unlock() {
    WORTHY_DCHECK((current_ == this) && isLocked());
    thread_id_ = std::thread::id();
    locked_.clear(std::memory_order_release);
}


} } // namespace worthy::internal
