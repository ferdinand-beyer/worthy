#include "internal/space.h"

#include "internal/block_allocator.h"
#include "internal/check.h"
#include "internal/generation.h"
#include "internal/object.h"

#include <boost/align/align_up.hpp>


namespace worthy {
namespace internal {


Space* Space::of(const Object* object) {
    BlockOwner* owner = Block::of(const_cast<Object*>(object))->owner();
    WORTHY_DCHECK(dynamic_cast<Space*>(owner));
    return static_cast<Space*>(owner);
}


Space::Space(Heap* heap, BlockAllocator* allocator,
        uint16_t generation_no, uint16_t block_flags) :
    heap_{heap},
    allocator_{allocator},
    generation_no_{generation_no},
    block_flags_{block_flags},
    next_generation_{nullptr},
    object_count_{0}
{
    WORTHY_DCHECK(heap);
    WORTHY_DCHECK(allocator);
}


Space::~Space() {
    allocator_->deallocateList(blocks_);
}


Heap* Space::heap() const {
    return heap_;
}


BlockAllocator& Space::allocator() {
    return *allocator_;
}


size_t Space::objectCount() const {
    return object_count_;
}


uint16_t Space::generationNumber() const {
    return generation_no_;
}


Generation* Space::nextGeneration() const {
    return next_generation_;
}


void Space::setNextGeneration(Generation* generation) {
    WORTHY_CHECK(!next_generation_ && blocks_.empty());
    next_generation_ = generation;
}


void Space::clear() {
    allocator_->deallocateList(blocks_);
    object_count_ = 0;
}


void Space::initBlock(Block& block) {
    block.setOwner(this);
    block.setGenerationNumber(generation_no_);
    block.setNextGenerationNumber(
            next_generation_ ? next_generation_->generationNumber()
                             : generation_no_);
    block.addFlags(block_flags_);
}


void* Space::allocate(size_t& size) {
    if (size >= LargeObjectThreshold) {
        WORTHY_UNIMPLEMENTED(); // TODO
    }

    // Allocate more to leave an aligned pointer.
    size = boost::alignment::align_up(size, ObjectAlignment);
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
    Block* block = allocator_->allocate(blocksForBytes(size));
    blocks_.push_front(*block);
    initBlock(*block);
    return block;
}


} } // namespace worthy::internal
