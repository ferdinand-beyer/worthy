#ifndef WORTHY_INTERNAL_GC_WORKER_H_
#define WORTHY_INTERNAL_GC_WORKER_H_


#include "internal/gc_visitor.h"
#include "internal/globals.h"


namespace worthy {
namespace internal {


class Block;
class GCWorkspace;
class GarbageCollector;


class GCWorker final : public GCVisitor {
public:
    GCWorker(const GCWorker&) = delete;
    GCWorker& operator=(const GCWorker&) = delete;

    GCWorker(GarbageCollector* gc, uint16_t generation_count);

    void prepareCycle();
    void executeCycle();

private:
    GCWorkspace& workspace(uint16_t generation_no);

    void doVisit(Object*& addr) override;

    void evacuate(Object*& addr);
    void copy(Object*& addr, uint16_t generation_no);
    void* allocate(size_t size, uint16_t generation_no);

    static void alreadyMoved(Object*& addr, Object* new_addr);

    void scavengeAll();
    bool tryScavengeAllocationBlock(GCWorkspace& workspace);

    void scavengeBlock(Block& block);

    void collectCompletedBlocks();

    GarbageCollector* const gc_;
    const uint16_t generation_count_;

    /// Block we are currently scanning.
    Block* scan_block_;

    /// Youngest generation for evacuate().
    uint16_t min_evac_generation_no_;

    /// Whether we may promote an object to the minimum evacuation
    /// generation instead of the intended generation.  This should
    /// be set for "lazy immutable" objects only, when we can assume
    /// that the young object will live as long as the old one.
    bool eager_promotion_;
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_GC_WORKER_H_
