#include "internal/heap.h"

#include "internal/check.h"

#include <algorithm>
#include <thread>


namespace worthy {
namespace internal {


Heap::Heap() :
    allocator_{},
    handle_pool_{&allocator_},
    eternity_{this, &allocator_},
    threads_{&allocator_}
{
    addThread();
}


Heap::~Heap() {
}


const Eternity& Heap::eternity() const {
    return eternity_;
}


HandlePtr Heap::makeHandle(Object* obj) {
    WORTHY_DCHECK(obj);
    return handle_pool_.makeHandle(obj);
}


void Heap::lock() {
    WORTHY_DCHECK(!isLocked());
    if (!tryLockFreeThread()) {
        // TODO: Block if we are at a safepoint.
        lockThreadSync();
    }
}


void Heap::unlock() {
    WORTHY_DCHECK(isLocked());

    Thread::current_thread_->unlock();
    thread_unlocked_.notify_one();
}


bool Heap::isLocked() const {
    if (auto thread = registeredThread()) {
        return thread->isLocked();
    }
    return false;
}


Thread* Heap::registeredThread() const {
    return (Thread::current_thread_ && (Thread::current_thread_->root_ == this))
            ? Thread::current_thread_ : nullptr;
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
    bool locked = addThread().tryLock();
    WORTHY_CHECK(locked);
}


bool Heap::tryLockFreeThread() {
    auto thread = registeredThread();
    // Try to lock the same thread as before first.
    const size_t start = thread ? thread->index_ : 0;
    for (size_t i = 0; i < threads_.size(); i++) {
        if (threads_[(start + i) % threads_.size()].tryLock()) {
            return true;
        }
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
