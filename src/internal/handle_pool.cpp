#include "internal/handle_pool.h"

#include "internal/block.h"
#include "internal/check.h"


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
