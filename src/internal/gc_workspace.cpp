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
    // TODO: Make sure that size is < LargeObjectThreshold.
    if (allocation_block_->bytesAvailable() >= size) {
        ++object_count_;
        return allocation_block_->allocate(size);
    }

    if (!allocation_block_->hasFlags(Block::Scanning)) {
        if (allocation_block_->scan_ptr_ == allocation_block_->free_) {
            WORTHY_CHECK(allocation_block_->free_ != allocation_block_->start_);
            completed_blocks_.push_back(*allocation_block_);
        } else {
            pending_blocks_.push_back(*allocation_block_);
        }
    }

    allocateBlock();

    ++object_count_;
    return allocation_block_->allocate(size);
}


void GCWorkspace::collectCompletedBlocks() {
    std::lock_guard<std::mutex> lock(generation_->mutex_);
    // TODO: Move completed blocks to the generation.
    //generation_->blocks_.splice(end(), completed_blocks_);
    generation_->object_count_ += object_count_;

    object_count_ = 0;
}


void GCWorkspace::allocateBlock() {
    // TODO: Use allocateSync()!
    // TODO: Set EvacuatedFlag?
    allocation_block_ = allocator_->allocate();
    allocation_block_->scan_ptr_ = allocation_block_->free_;
    generation_->initBlock(*allocation_block_);
}


} } // namespace worthy::internal
