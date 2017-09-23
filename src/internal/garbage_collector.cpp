#include "internal/garbage_collector.h"

#include "internal/frame.h"
#include "internal/generation.h"
#include "internal/heap.h"


namespace worthy {
namespace internal {


// Forwarding pointers: We could reorder the fields in Object
// and make the flags_ field first, then use the LSB to
// distinguish live objects from forwarding pointers.  This
// works as objects are 8-byte aligned, so the last three bits
// of object pointers (forwarding addresses) are always zero.
// Could be an 'Alive' bit.
// XXX: For parallel GC, we need an atomic operation.


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

    // Scavenge until there is nothing more to do.

    // Clean-up gens:
    // - Free old blocks (from-space)
    // Clean-up older gens:
    // - Record evacuated large objects

    // Reset nurseries
}


void GarbageCollector::prepareGenerations() {
    for (uint i = 0; i <= max_generation_index_; i++) {
        prepareCollectedGeneration(heap_->generations_[i]);
    }
    // TODO: Prepare older gens: stash remembered sets
}


void GarbageCollector::prepareCollectedGeneration(Generation& gen) {
    // Prepare gens:
    // - Stash blocks (to-space -> from-space)
    // - Flag blocks as not evacuated (from-space)
}


void GarbageCollector::evacuateRoots() {
    heap_->handle_pool_.accept(worker_);
}


} } // namespace worthy::internal
