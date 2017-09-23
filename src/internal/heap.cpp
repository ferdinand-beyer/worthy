#include "internal/heap.h"

#include "internal/block_layout.h"
#include "internal/check.h"
#include "internal/eternity.h"
#include "internal/frame.h"
#include "internal/garbage_collector.h"
#include "internal/generation.h"
#include "internal/handle_pool.h"

#include <algorithm>
#include <cstddef>
#include <thread>

#include <boost/align/align_up.hpp>


using boost::alignment::align_up;


namespace worthy {
namespace internal {


namespace {


constexpr uint MaxFramesPerThread = 8;
constexpr uint GenerationCount = 2;


template<typename T>
inline constexpr size_t alignedSizeOf() {
    return align_up(sizeof(T), alignof(std::max_align_t));
}


inline uint threadCount() {
    return std::max(1u, std::thread::hardware_concurrency());
}


template<typename T>
inline void destroy(T* ptr) {
    ptr->~T();
}


} // namespace


Heap::Heap() :
    thread_count_{threadCount()},
    max_frame_count_{thread_count_ * MaxFramesPerThread},
    allocator_{},
    frame_count_{0}
{
    // Calculate (maximum) required size for all member objects.
    const size_t reserve_size =
        alignedSizeOf<HandlePool>() +
        alignedSizeOf<Eternity>() +
        GenerationCount * alignedSizeOf<Generation>() +
        max_frame_count_ * alignedSizeOf<Frame>() +
        alignedSizeOf<GarbageCollector>() +
        thread_count_ * alignedSizeOf<GCWorker>();

    block_ = allocator_.allocate(blocksForBytes(reserve_size));

    WORTHY_DCHECK(block_->bytesAvailable() >= reserve_size);

    handle_pool_ = block_->construct<HandlePool>(&allocator_);
    eternity_ = block_->construct<Eternity>(this, &allocator_);

    initGenerations();

    gc_ = block_->construct<GarbageCollector>(this);
    // TODO: Allocate GC workers and workspaces.

    // After this point, the memory in the allocated block is exclusively
    // reserved for frames.
    initFrames();
}


Heap::~Heap() {
    for (uint i = 0; i < frame_count_; i++) {
        destroy(&frames_[i]);
    }

    destroy(gc_);

    for (uint i = 0; i < GenerationCount; i++) {
        destroy(&generations_[i]);
    }

    destroy(eternity_);
    destroy(handle_pool_);
}


const Eternity& Heap::eternity() const {
    return *eternity_;
}


HandlePtr Heap::makeHandle(Object* obj) {
    WORTHY_DCHECK(obj);
    return handle_pool_->makeHandle(obj);
}


void Heap::lock() {
    WORTHY_DCHECK(!isLocked());
    if (!tryAcquireFrame()) {
        // TODO: Block if we are at a safepoint.
        acquireFrameSync();
    }
}


void Heap::unlock() {
    WORTHY_DCHECK(isLocked());

    Frame::current_->unlock();
    frame_released_.notify_one();
}


size_t Heap::objectCount() const {
    size_t count = eternity_->objectCount();
    for (uint i = 0; i < GenerationCount; i++) {
        count += generations_[i].objectCount();
    }
    for (uint i = 0; i < frame_count_; i++) {
        count += frames_[i].nursery().objectCount();
    }
    return count;
}


void Heap::gc() {
    WORTHY_CHECK(!isLocked());
    // TODO: Stop the world, determine required generation number.
    gc_->collect(GenerationCount - 1);
}


void Heap::initGenerations() {
    generations_ = block_->construct<Generation>(0, this, &allocator_);
    for (uint i = 1; i < GenerationCount; i++) {
        block_->construct<Generation>(i, this, &allocator_);
        generations_[i - 1].setNextGeneration(&generations_[i]);
    }
    const uint last = GenerationCount - 1;
    generations_[last].setNextGeneration(&generations_[last]);
}


void Heap::initFrames() {
    frame_count_ = thread_count_;
    frames_ = block_->construct<Frame>(0, this, &allocator_);
    for (uint i = 1; i < frame_count_; i++) {
        block_->construct<Frame>(i, this, &allocator_);
    }
    for (uint i = 0; i < frame_count_; i++) {
        frames_[i].nursery().setNextGeneration(&generations_[0]);
    }
}


bool Heap::isLocked() const {
    if (auto frame = currentFrame()) {
        return frame->isLocked();
    }
    return false;
}


Frame* Heap::currentFrame() const {
    return (Frame::current_ && (Frame::current_->heap_ == this))
            ? Frame::current_ : nullptr;
}


Frame& Heap::newFrame() {
    WORTHY_DCHECK(frame_count_ < max_frame_count_);
    auto frame = block_->construct<Frame>(frame_count_, this, &allocator_);
    WORTHY_DCHECK(frame == &frames_[frame_count_]);
    ++frame_count_;
    frame->nursery().setNextGeneration(&generations_[0]);
    return *frame;
}


void Heap::acquireFrameSync() {
    std::unique_lock<std::mutex> lock(frames_mutex_);

    // If we reached the frame limit, we have to wait for one to become
    // available again.
    if (frame_count_ == max_frame_count_) {
        while (!tryAcquireFrame()) {
            frame_released_.wait(lock);
        }
        return;
    }

    // Try a free frame again, as we could have been blocked by garbage
    // collection before we acquired the mutex.
    if (tryAcquireFrame()) {
        return;
    }

    // Create and lock a new frame.
    bool locked = newFrame().tryLock();
    WORTHY_CHECK(locked);
}


bool Heap::tryAcquireFrame() {
    auto frame = currentFrame();
    // Try to lock the same frame as before first.
    const size_t start = frame ? frame->index_ : 0;
    for (size_t i = 0; i < frame_count_; i++) {
        if (frames_[(start + i) % frame_count_].tryLock()) {
            return true;
        }
    }
    return false;
}


} } // namespace worthy::internal
