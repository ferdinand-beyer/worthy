#include "internal/thread.h"

#include "internal/check.h"


namespace worthy {
namespace internal {


thread_local Thread* Thread::current_thread_ = nullptr;


Thread& Thread::current() {
    WORTHY_DCHECK(current_thread_ && current_thread_->isLocked());
    return *current_thread_;
}


Thread::Thread(size_t index, Heap* root, BlockAllocator* allocator, ConstructKey)
    : index_{index},
      root_{root},
      allocator_{allocator},
      nursery_{root, allocator} {
    locked_.clear(std::memory_order_relaxed);
}


bool Thread::isLocked() const {
    return thread_id_ != std::thread::id();
}


bool Thread::tryLock() {
    if (locked_.test_and_set(std::memory_order_acquire)) {
        // Was locked before.
        return false;
    }
    thread_id_ = std::this_thread::get_id();
    current_thread_ = this;
    return true;
}


void Thread::unlock() {
    thread_id_ = std::thread::id();
    locked_.clear(std::memory_order_release);
}


} } // namespace worthy::internal
