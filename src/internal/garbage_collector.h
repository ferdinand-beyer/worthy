#ifndef WORTHY_INTERNAL_GARBAGE_COLLECTOR_
#define WORTHY_INTERNAL_GARBAGE_COLLECTOR_


#include "internal/gc_worker.h"
#include "internal/globals.h"


namespace worthy {
namespace internal {


class Generation;
class Heap;


class GarbageCollector final {
public:
    GarbageCollector(const GarbageCollector&) = delete;
    GarbageCollector& operator=(const GarbageCollector&) = delete;

    explicit GarbageCollector(Heap* heap);

    void collect(size_t generation_index);

    // TODO: Temporary, required until we use workspaces.
    Generation& gen(uint16_t no);

private:
    void prepareGenerations();
    static void prepareCollectedGeneration(Generation& gen);
    static void swapSpaces(Generation& gen);

    void evacuateRoots();

    void finalizeGenerations();
    static void finalizeCollectedGeneration(Generation& gen);

    void resetNurseries();

    Heap* const heap_;

    GCWorker worker_;

    size_t max_generation_index_;
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_GARBAGE_COLLECTOR_
