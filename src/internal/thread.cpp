#include "internal/thread.h"

#include "internal/check.h"


namespace worthy {
namespace internal {


Thread::Thread(size_t index, Heap* root, BlockAllocator* allocator, ConstructKey)
    : index_{index},
      root_{root},
      allocator_{allocator},
      nursery_{root, allocator},
      state_{Free} {
}


bool Thread::tryLock() {
    State expected = Free;
    if (!state_.compare_exchange_strong(
                expected, Locked,
                std::memory_order_acquire,
                std::memory_order_relaxed)) {
        return false;
    }
    thread_id_ = std::this_thread::get_id();
    return true;
}


void Thread::unlock() {
    auto old_state = state_.exchange(Free, std::memory_order_release);
    WORTHY_CHECK(old_state == Locked);

    thread_id_ = std::thread::id();
}


bool Thread::isLocked() const {
    return state_.load(std::memory_order_relaxed) == Locked;
}


} } // namespace worthy::internal
