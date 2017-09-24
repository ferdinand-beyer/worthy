#ifndef WORTHY_INTERNAL_GC_WORKER_H_
#define WORTHY_INTERNAL_GC_WORKER_H_


#include "internal/block.h"
#include "internal/gc_visitor.h"
#include "internal/globals.h"


namespace worthy {
namespace internal {


class GarbageCollector;

struct GCWorkspace;


class alignas(64) GCWorker final : public GCVisitor {
public:
    GCWorker(const GCWorker&) = delete;
    GCWorker& operator=(const GCWorker&) = delete;

    explicit GCWorker(GarbageCollector* gc);

    void visit(Object*& addr) override;

    void scavenge();

private:
    GCWorkspace& workspace(uint16_t generation_index);

    void evacuate(Object*& addr);
    void copy(Object*& addr, uint16_t generation_number);
    void* allocate(size_t size, uint16_t generation_number);

    static void alreadyMoved(Object*& addr, Object* new_addr);

    GarbageCollector* const gc_;
};


struct alignas(64) GCWorkspace {
    BlockList done_blocks_;
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_GC_WORKER_H_
