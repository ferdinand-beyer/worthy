#include "internal/gc_workspace.h"

#include "internal/block_allocator.h"
#include "internal/check.h"
#include "internal/generation.h"


namespace worthy {
namespace internal {


namespace {


inline void transferBlocks(BlockList& from, BlockList& to) {
    to.splice(to.end(), from);
}


} // namespace


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


Block* GCWorkspace::popPendingBlock() {
    if (pending_blocks_.empty()) {
        return nullptr;
    }
    auto block = &pending_blocks_.front();
    pending_blocks_.pop_front();
    return block;
}


void* GCWorkspace::allocate(size_t size) {
    // TODO: Make sure that size is < LargeObjectThreshold.
    ensureAvailable(size);
    ++object_count_;
    return allocation_block_->allocate(size);
}


void GCWorkspace::collectCompletedBlocks() {
    std::lock_guard<std::mutex> lock(generation_->mutex_);
    transferBlocks(completed_blocks_, generation_->blocks_);
    generation_->object_count_ += object_count_;
    object_count_ = 0;
}


void GCWorkspace::ensureAvailable(size_t size) {
    if (allocation_block_->bytesAvailable() >= size) {
        return;
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
}


void GCWorkspace::allocateBlock() {
    // TODO: Use allocateSync()!
    allocation_block_ = allocator_->allocate();
    generation_->initBlock(*allocation_block_);
    // TODO: Set Evacuated flag
    allocation_block_->scan_ptr_ = allocation_block_->free_;
}


} } // namespace worthy::internal
