#include "internal/owner_block_allocator.h"

#include "internal/block_owner.h"
#include "internal/check.h"


namespace worthy {
namespace internal {


OwnerBlockAllocator::OwnerBlockAllocator(
        BlockOwner* owner, BlockAllocator* allocator)
    : owner_(owner), allocator_{allocator} {
    WORTHY_CHECK(allocator);
}


Block* OwnerBlockAllocator::allocate(size_t block_count) {
    Block* block = allocator_->allocate(block_count);
    block->setOwner(owner_);
    return block;
}


void OwnerBlockAllocator::deallocate(Block* block) {
    allocator_->deallocate(block);
}


} } // namespace worthy::internal
