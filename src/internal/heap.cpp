#include "internal/heap.h"

#include "internal/check.h"

#include <algorithm>
#include <thread>


namespace worthy {
namespace internal {


thread_local Thread* Heap::current_thread_ = nullptr;


Thread* Heap::currentThread() {
    WORTHY_DCHECK(current_thread_ && current_thread_->isLocked());
    return current_thread_;
}


Heap::Heap() :
    allocator_{},
    handle_pool_{&allocator_},
    eternity_{this, &allocator_},
    threads_{&allocator_},
    nursery_{this, &allocator_}
{
    addThread();
}


Heap::~Heap() {
}


const Eternity& Heap::eternity() const {
    return eternity_;
}


Nursery* Heap::nursery() {
    return &nursery_;
}


HandlePtr Heap::makeHandle(Object* obj) {
    WORTHY_DCHECK(obj);
    return handle_pool_.makeHandle(obj);
}


void Heap::lock() {
    if (!tryLockFreeThread()) {
        // TODO: Block if we are at a safepoint.
        lockThreadSync();
    }
    WORTHY_DCHECK(currentThreadIsValid() && current_thread_->isLocked());
}


void Heap::unlock() {
    WORTHY_DCHECK(currentThreadIsValid() && current_thread_->isLocked());

    current_thread_->unlock();
    thread_unlocked_.notify_one();
}


bool Heap::currentThreadIsValid() const {
    return current_thread_ && (current_thread_->root_ == this);
}


Thread& Heap::addThread() {
    return threads_.emplace_back(
            threads_.size(), this, &allocator_, Thread::ConstructKey());
}


void Heap::lockThreadSync() {
    std::unique_lock<std::mutex> lock(threads_mutex_);

    // If we reached the thread limit, we have to wait for one thread to
    // become available again.
    if (threads_.size() == maxThreadCount()) {
        while (!tryLockFreeThread()) {
            thread_unlocked_.wait(lock);
        }
        return;
    }

    // Try a free thread again, as we could have been blocked by garbage
    // collection.
    if (tryLockFreeThread()) {
        return;
    }

    // Create and lock a new thread.
    bool locked = tryLockThread(addThread());
    WORTHY_CHECK(locked);
}


bool Heap::tryLockFreeThread() {
    // If current_thread_ is set, this thread had acquired a thread context
    // before.  Try this previous one first.
    const size_t start = currentThreadIsValid() ? current_thread_->index_ : 0;
    for (size_t i = 0; i < threads_.size(); i++) {
        if (tryLockThread(threads_[(start + i) % threads_.size()])) {
            return true;
        }
    }
    return false;
}


bool Heap::tryLockThread(Thread& thread) {
    if (thread.tryLock()) {
        current_thread_ = &thread;
        return true;
    }
    return false;
}


size_t Heap::maxThreadCount() {
    static size_t result = 0;
    if (!result) {
        result = 8 * std::max(1u, std::thread::hardware_concurrency());
    }
    return result;
}


} } // namespace worthy::internal
