#ifndef WORTHY_INTERNAL_THREAD_H_
#define WORTHY_INTERNAL_THREAD_H_


#include "internal/globals.h"
#include "internal/nursery.h"

#include <atomic>
#include <thread>


namespace worthy {
namespace internal {


class BlockAllocator;
class Heap;


class Thread final {
    struct ConstructKey {};

public:
    static Thread& current();

    Thread(const Thread&) = delete;
    Thread& operator=(const Thread&) = delete;

    Thread(size_t index, Heap* root, BlockAllocator* allocator, ConstructKey);

    Heap& root();

    Nursery& nursery();

private:
    static thread_local Thread* current_thread_;

    bool isLocked() const;

    bool tryLock();
    void unlock();

    const size_t index_;

    Heap* const root_;
    BlockAllocator* const allocator_;
    Nursery nursery_;

    std::atomic_flag locked_;

    std::thread::id thread_id_;

    friend class Heap;
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_THREAD_H_
