#ifndef WORTHY_INTERNAL_HEAP_H_
#define WORTHY_INTERNAL_HEAP_H_


#include "internal/blocked_vector.h"
#include "internal/eternity.h"
#include "internal/globals.h"
#include "internal/handle_pool.h"
#include "internal/nursery.h"
#include "internal/root_block_allocator.h"
#include "internal/thread.h"

#include <condition_variable>
#include <mutex>


namespace worthy {
namespace internal {


class HashMap;
class HashMapBitmapNode;
class Object;


class Heap final {
public:
    Heap(const Heap&) = delete;
    Heap& operator=(const Heap&) = delete;

    Heap();
    ~Heap();

    const Eternity& eternity() const;

    Nursery* nursery();

    HandlePtr makeHandle(Object* obj);

    void lock();
    void unlock();

    bool isLocked() const;

private:
    Thread* registeredThread() const;

    Thread& addThread();

    void lockThreadSync();
    bool tryLockFreeThread();

    static size_t maxThreadCount();

    RootBlockAllocator allocator_;
    HandlePool handle_pool_;
    Eternity eternity_;

    BlockedVector<Thread> threads_;

    std::mutex threads_mutex_;

    std::condition_variable thread_unlocked_;

    // TODO: Per-thread
    Nursery nursery_;
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_HEAP_H_
