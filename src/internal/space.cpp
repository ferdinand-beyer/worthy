#include "internal/space.h"

#include "internal/block_allocator.h"
#include "internal/check.h"

#include <boost/align/align_up.hpp>


using boost::alignment::align_up;


namespace worthy {
namespace internal {


Space* Space::of(const Object* object) {
    BlockOwner* owner = Block::of(const_cast<Object*>(object))->owner();
    WORTHY_DCHECK(dynamic_cast<Space*>(owner));
    return static_cast<Space*>(owner);
}


Space::Space(Heap* heap, BlockAllocator* allocator)
    : heap_{heap},
      allocator_{allocator} {
    WORTHY_DCHECK(heap);
    WORTHY_DCHECK(allocator);
}


Space::~Space() {
    allocator_->deallocateList(blocks_);
}


Heap* Space::heap() const {
    return heap_;
}


void* Space::allocate(size_t size) {
    Block* block = blockForAllocation(size);
    return block->allocate(size);
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
    return block;
}


} } // namespace worthy::internal
