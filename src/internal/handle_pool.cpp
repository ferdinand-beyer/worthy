#include "internal/handle_pool.h"

#include "internal/block.h"
#include "internal/check.h"
#include "internal/gc_visitor.h"


namespace worthy {
namespace internal {


HandlePool::HandlePool(BlockAllocator* allocator) :
    allocator_{this, allocator},
    handles_{&allocator_} {
}


HandlePtr HandlePool::makeHandle(Object* obj) {
    Handle* handle = free_handles_.pop();
    if (handle) {
        WORTHY_DCHECK(handle->ref_count_ == 0);
        handle->obj_ = obj;
        return handle;
    }
    return &handles_.emplace_back(obj, Handle::ConstructKey());
}


void HandlePool::accept(GCVisitor& visitor) {
    for (auto& handle : handles_) {
        // XXX: Use a dedicated field for lock-free decision if this handle is
        // free.  The GC has stopped the world.
        if (handle.ref_count_.load(std::memory_order_relaxed) > 0) {
            visitor.visit(handle.obj_);
        }
    }
}


void HandlePool::reclaim(Handle* handle) {
    WORTHY_DCHECK(handle->ref_count_ == 0);
    free_handles_.push(handle);
}


void HandlePoolAccess::reclaim(Handle* handle) {
    auto owner = Block::of(handle)->owner();
    WORTHY_DCHECK(dynamic_cast<HandlePool*>(owner));
    HandlePool* pool = static_cast<HandlePool*>(owner);
    pool->reclaim(handle);
}


} } // namespace worthy::internal
