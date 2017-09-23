#include "internal/gc_worker.h"

#include "internal/garbage_collector.h"


namespace worthy {
namespace internal {


GCWorker::GCWorker(GarbageCollector* gc)
    : gc_{gc} {
}


void GCWorker::visit(Object*& reference) {
}


} } // namespace worthy::internal
