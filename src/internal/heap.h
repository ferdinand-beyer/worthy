#ifndef WORTHY_INTERNAL_HEAP_H_
#define WORTHY_INTERNAL_HEAP_H_


#include "internal/globals.h"
#include "internal/handle.h"
#include "internal/root_block_allocator.h"

#include <condition_variable>
#include <mutex>


namespace worthy {
namespace internal {


class Eternity;
class Frame;
class GarbageCollector;
class Generation;
class HandlePool;
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

    size_t objectCount() const;

    void gc();

private:
    void initGenerations();
    void initFrames();

    bool isLocked() const;

    Frame* currentFrame() const;
    Frame& newFrame();

    void acquireFrameSync();
    bool tryAcquireFrame();

    const uint generation_count_;
    const uint cpu_count_;
    const uint max_frame_count_;

    RootBlockAllocator allocator_;
    Block* workspace_;

    HandlePool* handle_pool_;
    Eternity* eternity_;
    Generation* generations_;
    GarbageCollector* gc_;

    Frame* frames_;
    size_t frame_count_;

    std::mutex frames_mutex_;
    std::condition_variable frame_released_;

    friend class GarbageCollector;
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_HEAP_H_
