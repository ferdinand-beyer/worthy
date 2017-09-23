#ifndef WORTHY_INTERNAL_FRAME_H_
#define WORTHY_INTERNAL_FRAME_H_


#include "internal/globals.h"
#include "internal/nursery.h"

#include <atomic>
#include <thread>


namespace worthy {
namespace internal {


class BlockAllocator;
class Heap;


class Frame final {
public:
    static Frame& current();

    Frame(const Frame&) = delete;
    Frame& operator=(const Frame&) = delete;

    Frame(size_t index, Heap* heap, BlockAllocator* allocator);

    Heap& heap();
    Nursery& nursery();
    const Nursery& nursery() const;

private:
    static thread_local Frame* current_;

    bool isLocked() const;

    bool tryLock();
    void unlock();

    const size_t index_;

    Heap* const heap_;
    BlockAllocator* const allocator_;
    Nursery nursery_;

    std::atomic_flag locked_ = ATOMIC_FLAG_INIT;

    std::thread::id thread_id_;

    friend class Heap;
};


template<typename T, typename... Args>
inline T* construct(Args&&... args) {
    return Frame::current().nursery().construct<T>(
            std::forward<Args>(args)...);
}


template<typename T, typename... Args>
inline T* constructEx(size_t size, Args&&... args) {
    return Frame::current().nursery().constructEx<T>(
            size, std::forward<Args>(args)...);
}


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_FRAME_H_
