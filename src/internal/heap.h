#ifndef WORTHY_INTERNAL_HEAP_H_
#define WORTHY_INTERNAL_HEAP_H_


#include "internal/blocked_vector.h"
#include "internal/eternity.h"
#include "internal/globals.h"
#include "internal/handle_pool.h"
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

    HandlePtr makeHandle(Object* obj);

    void lock();
    void unlock();

private:
    bool isLocked() const;

    Thread* registeredThread() const;
    Thread& addThread();

    void acquireThreadSync();
    bool tryAcquireThread();

    static size_t maxThreadCount();

    RootBlockAllocator allocator_;
    HandlePool handle_pool_;
    Eternity eternity_;

    BlockedVector<Thread> threads_;

    std::mutex threads_mutex_;

    std::condition_variable thread_released_;
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_HEAP_H_
