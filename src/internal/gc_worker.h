#ifndef WORTHY_INTERNAL_GC_WORKER_H_
#define WORTHY_INTERNAL_GC_WORKER_H_


#include "internal/gc_visitor.h"


namespace worthy {
namespace internal {


class GarbageCollector;


class GCWorker final : public GCVisitor {
public:
    GCWorker(const GCWorker&) = delete;
    GCWorker& operator=(const GCWorker&) = delete;

    explicit GCWorker(GarbageCollector* gc);

    void visit(Object*& reference) override;

private:
    GarbageCollector* gc_;
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_GC_WORKER_H_
