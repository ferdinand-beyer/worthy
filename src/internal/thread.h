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
    Thread(const Thread&) = delete;
    Thread& operator=(const Thread&) = delete;

    Thread(size_t index, Heap* root, BlockAllocator* allocator, ConstructKey);

    Nursery& nursery();

private:
    enum State {
        Free,
        Locked,
        Blocked
    };

    bool tryLock();
    void unlock();

    bool isLocked() const;

    const size_t index_;

    Heap* const root_;
    BlockAllocator* const allocator_;
    Nursery nursery_;

    std::atomic<State> state_;

    std::thread::id thread_id_;

    friend class Heap;
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_THREAD_H_
