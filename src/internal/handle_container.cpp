#include "internal/handle_container.h"

#include "internal/block.h"
#include "internal/check.h"


namespace worthy {
namespace internal {


HandleContainer::HandleContainer(BlockAllocator* allocator) :
    allocator_{this, allocator},
    handles_{&allocator_} {
}


HandlePtr HandleContainer::newHandle(Object* obj) {
    Handle* handle = free_handles_.pop();
    if (handle) {
        WORTHY_DCHECK(handle->ref_count_ == 0);
        handle->obj_ = obj;
        return handle;
    }
    return &handles_.emplace_back(obj, Handle::ConstructKey());
}


void HandleContainer::reclaim(Handle* handle) {
    WORTHY_DCHECK(handle->ref_count_ == 0);
    free_handles_.push(handle);
}


void HandleAccess::reclaim(Handle* handle) {
    auto owner = Block::of(handle)->owner();
    WORTHY_DCHECK(dynamic_cast<HandleContainer*>(owner));
    HandleContainer* container = static_cast<HandleContainer*>(owner);
    container->reclaim(handle);
}


} } // namespace worthy::internal
