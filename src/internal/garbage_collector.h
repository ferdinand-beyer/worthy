#ifndef WORTHY_INTERNAL_GARBAGE_COLLECTOR_
#define WORTHY_INTERNAL_GARBAGE_COLLECTOR_


#include "internal/gc_worker.h"
#include "internal/globals.h"


namespace worthy {
namespace internal {


class Block;
class Generation;
class Heap;


class GarbageCollector final {
public:
    GarbageCollector(const GarbageCollector&) = delete;
    GarbageCollector& operator=(const GarbageCollector&) = delete;

    explicit GarbageCollector(Heap* heap);
    ~GarbageCollector();

    void collect(size_t generation_index);

    // TODO: Temporary, required until we use workspaces.
    Generation& gen(uint16_t no);

private:
    GCWorker* worker(size_t index) const;

    void prepareGenerations();
    static void prepareCollectedGeneration(Generation& gen);
    static void swapSpaces(Generation& gen);

    void evacuateRoots();

    void finalizeGenerations();
    static void finalizeCollectedGeneration(Generation& gen);

    void resetNurseries();

    Heap* const heap_;
    const size_t worker_count_;
    const size_t worker_size_;

    Block* workspace_;

    size_t max_generation_index_;
    GCWorker* worker_;
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_GARBAGE_COLLECTOR_
