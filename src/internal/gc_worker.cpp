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
      scan_block_{nullptr},
      min_evac_generation_no_{0},
      eager_promotion_{false} {
}


void GCWorker::prepareCycle() {
}


void GCWorker::executeCycle() {
    scavengeAll();

    collectCompletedBlocks();
}


GCWorkspace& GCWorker::workspace(uint16_t generation_no) {
    GCWorkspace* workspaces = reinterpret_cast<GCWorkspace*>(this + 1);
    return workspaces[generation_no];
}


void GCWorker::doVisit(Object*& addr) {
    evacuate(addr);
}


void GCWorker::evacuate(Object*& addr) {
    Block* block = Block::of(addr);
    WORTHY_DCHECK(dynamic_cast<Space*>(block->owner()));

    if (block->flags() & Block::EternalFlag) {
        return;
    }
    if (block->flags() & Block::EvacuatedFlag) {
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
    // For oldest to youngest generation:
    // - scavenge current alloc block if (scan_ptr < alloc_ptr)
    // - scavenge large objects
    // - scavenge a pending block

    bool did_something = true;

    do {
        did_something = false;

        for (int i = generation_count_ - 1; i >= 0; i--) {
            auto& ws = workspace(i);

            if (tryScavengeAllocationBlock(ws)) {
                did_something = true;
                break;
            }
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

    scan_block_ = &block;
    min_evac_generation_no_ = block.generation_no_;
    //failed_to_evac = false

    auto& ws = workspace(block.generation_no_);

    while ((&block == &ws.allocationBlock()) &&
            (block.scan_ptr_ < block.free_)) {
        Object* object = reinterpret_cast<Object*>(block.scan_ptr_);
        object->traverse(*this);

        // TODO: Remembered set if failed to evac

        block.scan_ptr_ += object->size_in_words_ * WordSize;
    }

    WORTHY_DCHECK(block.scan_ptr_ == block.free_);

    // TODO: What can pull our block away here?

    scan_block_ = nullptr;
}


void GCWorker::collectCompletedBlocks() {
    for (uint16_t i = 0; i < generation_count_; i++) {
        workspace(i).collectCompletedBlocks();
    }
}


} } // namespace worthy::internal
