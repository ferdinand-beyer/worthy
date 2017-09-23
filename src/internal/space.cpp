#include "internal/space.h"

#include "internal/block_allocator.h"
#include "internal/check.h"
#include "internal/object.h"

#include <boost/align/align_up.hpp>


using boost::alignment::align_up;


namespace worthy {
namespace internal {


Space* Space::of(const Object* object) {
    BlockOwner* owner = Block::of(const_cast<Object*>(object))->owner();
    WORTHY_DCHECK(dynamic_cast<Space*>(owner));
    return static_cast<Space*>(owner);
}


Space::Space(Heap* heap, BlockAllocator* allocator, uint16_t generation_number,
        uint16_t block_flags)
    : heap_{heap},
      allocator_{allocator},
      block_flags_{block_flags},
      generation_number_{generation_number},
      next_generation_{nullptr},
      object_count_{0} {
    WORTHY_DCHECK(heap);
    WORTHY_DCHECK(allocator);
}


Space::~Space() {
    allocator_->deallocateList(blocks_);
}


Heap* Space::heap() const {
    return heap_;
}


size_t Space::objectCount() const {
    return object_count_;
}


void Space::setNextGeneration(Generation* generation) {
    next_generation_ = generation;
}


void Space::reset() {
    allocator_->deallocateList(blocks_);
    object_count_ = 0;
}


void* Space::allocate(size_t& size) {
    if (size >= LargeObjectThreshold) {
        WORTHY_UNIMPLEMENTED(); // TODO
    }
    // Allocate more to leave an aligned pointer.
    size = align_up(size, ObjectAlignment);
    Block* block = blockForAllocation(size);
    void* ptr = block->allocate(size);
    ++object_count_;
    return ptr;
}


Block* Space::blockForAllocation(size_t size) {
    if (!blocks_.empty()) {
        Block* block = &blocks_.front();
        if (block->bytesAvailable() >= size) {
            return block;
        }
    }
    const size_t count = align_up(size, BlockSize) / BlockSize;
    Block* block = allocator_->allocate(count);
    blocks_.push_front(*block);
    block->setOwner(this);
    block->flags() = block_flags_;
    return block;
}


} } // namespace worthy::internal
