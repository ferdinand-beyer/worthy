#include "internal/gc_worker.h"

#include "internal/block.h"
#include "internal/garbage_collector.h"
#include "internal/gc_workspace.h"
#include "internal/generation.h"
#include "internal/object.h"
#include "internal/space.h"

#include <cstring>


namespace worthy {
namespace internal {


GCWorker::GCWorker(GarbageCollector* gc, uint16_t generation_count)
    : gc_{gc},
      generation_count_{generation_count},
      min_evac_generation_no_{0},
      eager_promotion_{false} {
}


GCWorkspace& GCWorker::workspace(uint16_t generation_no) {
    GCWorkspace* workspaces = reinterpret_cast<GCWorkspace*>(this + 1);
    return workspaces[generation_no];
}


void GCWorker::prepareCycle() {
}


void GCWorker::executeCycle() {
    // Roots have already been evacuated, now scavenge and evacuate
    // reachable objects until we find no more.
    scavengeAll();

    collectCompletedBlocks();

    // TODO: Keep track of running workers; if other workers are still
    // collecting, we should wait for more work here.
}


void GCWorker::doVisit(Object*& addr) {
    evacuate(addr);
}


void GCWorker::evacuate(Object*& addr) {
    Block* block = Block::of(addr);
    WORTHY_DCHECK(dynamic_cast<Space*>(block->owner()));

    if (block->hasFlags(Block::Eternal)) {
        return;
    }
    if (block->hasFlags(Block::Evacuated)) {
        // Either an object of an older generation we are not collecting,
        // or an object in "to space".
        // TODO: Set failed_to_evac if < min_evac_generation_no_
        WORTHY_UNIMPLEMENTED(); // TODO
    }

    Space* space = static_cast<Space*>(block->owner());
    WORTHY_DCHECK(space->nextGeneration()
            && space->nextGeneration()->generationNumber() ==
                block->nextGenerationNumber());

    if (auto new_addr = addr->moved_.load(std::memory_order_relaxed)) {
        alreadyMoved(addr, new_addr);
        return;
    }

    copy(addr, block->nextGenerationNumber());
}


void GCWorker::copy(Object*& addr, uint16_t generation_no) {
    const size_t size = addr->size_in_words_ * WordSize;

    void* ptr = allocate(size, generation_no);
    std::memcpy(ptr, addr, size);

    Object* prev_addr = nullptr;
    Object* new_addr = reinterpret_cast<Object*>(ptr);

    if (addr->moved_.compare_exchange_strong(
                prev_addr, new_addr, std::memory_order_relaxed)) {
        addr = new_addr;
    } else {
        // Another worker has already evacuated this one.  The copy we just
        // made is now garbage and will be collected in the next cycle.
        alreadyMoved(addr, prev_addr);
    }
}


void* GCWorker::allocate(size_t size, uint16_t generation_no) {
#if 0
    if (generation_no < min_evac_generation_no_) {
        if (eager_promotion_) {
            generation_no = min_evac_generation_no_;
        } else {
            // TODO: Failed to evac/promote
        }
    }
#endif
    return workspace(generation_no).allocate(size);
}


void GCWorker::alreadyMoved(Object*& addr, Object* new_addr) {
    // TODO: Check generation numbers for remembered sets!
    addr = new_addr;
}


void GCWorker::scavengeAll() {
    bool did_something;

    do {
        did_something = false;

        for (int i = generation_count_ - 1; i >= 0; i--) {
            auto& ws = workspace(i);

            // Recently evacuated objects in this generation.
            if (tryScavengeAllocationBlock(ws)) {
                did_something = true;
                break;
            }

            // TODO: scavenge large objects
            // TODO: scavenge a pending block
        }
    } while (did_something);
}


bool GCWorker::tryScavengeAllocationBlock(GCWorkspace& workspace) {
    auto& block = workspace.allocationBlock();
    if (block.scan_ptr_ < block.free_) {
        scavengeBlock(block);
        return true;
    }
    return false;
}


void GCWorker::scavengeBlock(Block& block) {
    WORTHY_DCHECK(block.scan_ptr_ >= block.start_);
    WORTHY_DCHECK(!block.hasFlags(Block::Scanning));

    block.addFlags(Block::Scanning);

    min_evac_generation_no_ = block.generation_no_;
    //failed_to_evac = false

    auto& ws = workspace(block.generation_no_);

    while (block.scan_ptr_ < block.free_) {
        // TODO: Use object flags for optimization here:
        // - Can (does?) the object contain pointers (= need to traverse)?
        // - Set eager_promotion_ according to the object's mutability
        Object* object = reinterpret_cast<Object*>(block.scan_ptr_);
        object->traverse(*this);

        // TODO: If failed_to_evac, add this object to the remembered set.

        block.scan_ptr_ += object->size_in_words_ * WordSize;
    }

    WORTHY_DCHECK(block.scan_ptr_ == block.free_);

    block.removeFlags(Block::Scanning);
}


void GCWorker::collectCompletedBlocks() {
    for (uint16_t i = 0; i < generation_count_; i++) {
        workspace(i).collectCompletedBlocks();
    }
}


} } // namespace worthy::internal
