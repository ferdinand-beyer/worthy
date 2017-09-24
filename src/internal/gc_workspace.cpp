#include "internal/gc_workspace.h"

#include "internal/block_allocator.h"
#include "internal/check.h"
#include "internal/generation.h"


namespace worthy {
namespace internal {


GCWorkspace::GCWorkspace(Generation* generation, BlockAllocator* allocator) :
    generation_{generation},
    allocator_{allocator},
    allocation_block_{nullptr},
    object_count_{0}
{
    allocateBlock();
}


Block& GCWorkspace::allocationBlock() const {
    WORTHY_DCHECK(allocation_block_);
    return *allocation_block_;
}


void* GCWorkspace::allocate(size_t size) {
    // TODO: Check for large object!
    if (allocation_block_->bytesAvailable() >= size) {
        ++object_count_;
        return allocation_block_->allocate(size);
    }
    WORTHY_UNIMPLEMENTED(); // TODO
}


void GCWorkspace::collectCompletedBlocks() {
    std::lock_guard<std::mutex> lock(generation_->mutex_);
    // TODO: Move done blocks.
    generation_->object_count_ += object_count_;

    object_count_ = 0;
}


void GCWorkspace::allocateBlock() {
    // TODO: Sync!
    allocation_block_ = allocator_->allocate();
    allocation_block_->scan_ptr_ = allocation_block_->free_;
    generation_->registerBlock(*allocation_block_);
}


} } // namespace worthy::internal
