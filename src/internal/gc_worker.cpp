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


void GCWorker::visit(Object*& addr) {
    evacuate(addr);
}


void GCWorker::prepareCycle() {
}


void GCWorker::executeCycle() {
    // TODO: Scavenge all

    collectCompletedBlocks();
}


GCWorkspace& GCWorker::workspace(uint16_t generation_no) {
    GCWorkspace* workspaces = reinterpret_cast<GCWorkspace*>(this + 1);
    return workspaces[generation_no];
}


void GCWorker::evacuate(Object*& addr) {
    Block* block = Block::of(addr);
    WORTHY_DCHECK(dynamic_cast<Space*>(block->owner()));

    if (block->flags() & Block::ImmortalFlag) {
        return;
    }
    if (block->flags() & Block::EvacuatedFlag) {
        WORTHY_UNIMPLEMENTED(); // TODO
    }

    Space* space = static_cast<Space*>(block->owner());
    WORTHY_DCHECK(space->next_generation_);

    if (auto new_addr = addr->moved_.load(std::memory_order_relaxed)) {
        alreadyMoved(addr, new_addr);
        return;
    }

    // TODO: Cache generation number in block
    copy(addr, space->next_generation_->generation_number_);
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


void GCWorker::collectCompletedBlocks() {
    for (uint16_t i = 0; i < generation_count_; i++) {
        workspace(i).collectCompletedBlocks();
    }
}


} } // namespace worthy::internal
