#include "internal/garbage_collector.h"

#include "internal/block.h"
#include "internal/check.h"
#include "internal/frame.h"
#include "internal/generation.h"
#include "internal/handle_pool.h"
#include "internal/heap.h"
#include "internal/memory_utils.h"


namespace worthy {
namespace internal {


GarbageCollector::GarbageCollector(Heap* heap) :
    heap_{heap},
    worker_count_{heap->cpu_count_},
    worker_size_{sizeof(GCWorker) +
        heap->generation_count_ * sizeof(GCWorkspace)},
    max_generation_index_{0},
    worker_{nullptr}
{
    workspace_ = heap->allocator_.allocate(
            blocksForBytes(worker_count_ * worker_size_));

    for (uint i = 0; i < worker_count_; i++) {
        workspace_->construct<GCWorker>(this);
        for (uint k = 0; k < heap->generation_count_; k++) {
            workspace_->construct<GCWorkspace>();
        }
    }
}


GarbageCollector::~GarbageCollector() {
    heap_->allocator_.deallocate(workspace_);
}


GCWorker* GarbageCollector::worker(size_t index) const {
    return reinterpret_cast<GCWorker*>(
            workspace_->begin() + index * worker_size_);
}


void GarbageCollector::collect(size_t generation_index) {
    WORTHY_DCHECK(!worker_);

    max_generation_index_ = generation_index;
    worker_ = worker(0);

    prepareGenerations();

    //worker_->prepareCycle();

    // TODO: Scavenge old remembered sets

    evacuateRoots();

    worker_->scavenge();

    finalizeGenerations();
    resetNurseries();

    worker_ = nullptr;
}


void GarbageCollector::prepareGenerations() {
    for (uint i = 0; i <= max_generation_index_; i++) {
        prepareCollectedGeneration(heap_->generations_[i]);
    }
    // TODO: Prepare older gens: stash remembered sets
}


void GarbageCollector::prepareCollectedGeneration(Generation& gen) {
    // TODO: Dispose remembered sets (for each frame)
    swapSpaces(gen);
}


void GarbageCollector::swapSpaces(Generation& gen) {
    WORTHY_DCHECK(gen.old_blocks_.empty());
    gen.blocks_.swap(gen.old_blocks_);

    for (auto& block : gen.old_blocks_) {
        block.flags() &= ~Block::EvacuatedFlag;
    }

    gen.object_count_ = 0;
}


void GarbageCollector::evacuateRoots() {
    heap_->handle_pool_->accept(*worker_);
}


void GarbageCollector::finalizeGenerations() {
    for (uint i = 0; i <= max_generation_index_; i++) {
        finalizeCollectedGeneration(heap_->generations_[i]);
    }
    // TODO: Clean-up older gens:
    // - Move evacuated large objects
}


void GarbageCollector::finalizeCollectedGeneration(Generation& gen) {
    gen.allocator_->deallocateList(gen.old_blocks_);
}


void GarbageCollector::resetNurseries() {
    for (uint i = 0; i < heap_->frame_count_; i++) {
        heap_->frames_[i].nursery().reset();
        // TODO: Reserve new nursery space?
    }
}


// TODO: Remove
Generation& GarbageCollector::gen(uint16_t no) {
    return heap_->generations_[no];
}


} } // namespace worthy::internal
