#include "internal/garbage_collector.h"

#include "internal/block.h"
#include "internal/check.h"
#include "internal/frame.h"
#include "internal/generation.h"
#include "internal/heap.h"


namespace worthy {
namespace internal {


GarbageCollector::GarbageCollector(Heap* heap)
    : heap_{heap},
      worker_{this},
      max_generation_index_{0} {
    // TODO: Initialize workspace per Generation
}


void GarbageCollector::collect(size_t generation_index) {
    max_generation_index_ = generation_index;

    prepareGenerations();

    //worker_.prepareCycle();

    // TODO: Scavenge old remembered sets

    evacuateRoots();

    // TODO: Scavenge until there is nothing more to do.

    // TODO: Clean-up gens:
    // - Free old blocks (from-space)
    // Clean-up older gens:
    // - Move evacuated large objects

    resetNurseries();
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
    heap_->handle_pool_.accept(worker_);
}


void GarbageCollector::resetNurseries() {
    for (auto& frame : heap_->frames_) {
        frame.nursery().reset();
        // TODO: Reserve new nursery space?
    }
}


// TODO: Remove
Generation& GarbageCollector::gen(uint16_t no) {
    return heap_->generations_[no];
}


} } // namespace worthy::internal
