#include "internal/heap.h"

#include "internal/check.h"

#include <algorithm>
#include <thread>


namespace worthy {
namespace internal {


namespace {


constexpr uint MaxFramesPerThread = 8;
constexpr uint GenerationCount = 2;


inline uint threadCount() {
    return std::max(1u, std::thread::hardware_concurrency());
}


} // namespace


Heap::Heap() :
    thread_count_{threadCount()},
    max_frame_count_{thread_count_ * MaxFramesPerThread},
    allocator_{},
    handle_pool_{&allocator_},
    eternity_{this, &allocator_},
    generations_{&allocator_},
    frames_{&allocator_},
    gc_{this}
{
    initGenerations();
    initFrames();
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
    size_t count = eternity_.objectCount();
    for (auto& gen : generations_) {
        count += gen.objectCount();
    }
    for (auto& frame : frames_) {
        count += frame.nursery().objectCount();
    }
    return count;
}


void Heap::gc() {
    WORTHY_CHECK(!isLocked());
    // TODO: Stop the world, determine required generation number.
    gc_.collect(generations_.size() - 1);
}


void Heap::initGenerations() {
    for (uint i = 0; i < GenerationCount; i++) {
        generations_.emplace_back(i, this, &allocator_);
    }
    const uint last = GenerationCount - 1;
    for (uint i = 0; i < last; i++) {
        generations_[i].setNextGeneration(&generations_[i + 1]);
    }
    generations_[last].setNextGeneration(&generations_[last]);
}


void Heap::initFrames() {
    for (uint i = 0; i < thread_count_; i++) {
        newFrame();
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
    WORTHY_DCHECK(frames_.size() < max_frame_count_);
    auto& frame = frames_.emplace_back(frames_.size(), this, &allocator_);
    frame.nursery().setNextGeneration(&generations_.front());
    return frame;
}


void Heap::acquireFrameSync() {
    std::unique_lock<std::mutex> lock(frames_mutex_);

    // If we reached the frame limit, we have to wait for one to become
    // available again.
    if (frames_.size() == max_frame_count_) {
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
    for (size_t i = 0; i < frames_.size(); i++) {
        if (frames_[(start + i) % frames_.size()].tryLock()) {
            return true;
        }
    }
    return false;
}


} } // namespace worthy::internal
