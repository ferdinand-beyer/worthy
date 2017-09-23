#include "internal/gc_worker.h"

#include "internal/block.h"
#include "internal/garbage_collector.h"
#include "internal/generation.h"
#include "internal/object.h"
#include "internal/space.h"

#include <cstring>


namespace worthy {
namespace internal {


GCWorker::GCWorker(GarbageCollector* gc)
    : gc_{gc} {
    // TODO: Take a pointer to an array of workspaces, that must be
    // pre-allocated by the caller (= GarbageCollector).
}


void GCWorker::visit(Object*& addr) {
    evacuate(addr);
}


void GCWorker::scavenge() {
}


GCWorkspace& GCWorker::workspace(uint16_t generation_number) {
    GCWorkspace* workspaces = reinterpret_cast<GCWorkspace*>(this + 1);
    return workspaces[generation_number];
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

    copy(addr, space->next_generation_->generation_number_);
}


void GCWorker::copy(Object*& addr, uint16_t generation_number) {
    const size_t size = addr->size_in_words_ * WordSize;

    void* ptr = allocate(size, generation_number);
    std::memcpy(ptr, addr, size);

    Object* prev_addr = nullptr;
    Object* new_addr = reinterpret_cast<Object*>(ptr);

    if (addr->moved_.compare_exchange_strong(
                prev_addr, new_addr, std::memory_order_relaxed)) {
        addr = new_addr;
    } else {
        // Another thread has already evacuated this one.  The copy we just
        // made is garbage and will be collected in the next cycle.
        alreadyMoved(addr, prev_addr);
    }
}


void* GCWorker::allocate(size_t size, uint16_t generation_number) {
    // TODO: Assert for size < LargeObjectThreshold
    // TODO: Use workspace for allocations.
    auto& gen = gc_->gen(generation_number);
    return gen.allocate(size);
}


void GCWorker::alreadyMoved(Object*& addr, Object* new_addr) {
    // TODO: Check generation numbers for remembered sets!
    addr = new_addr;
}


} } // namespace worthy::internal
