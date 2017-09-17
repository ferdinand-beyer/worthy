#ifndef WORTHY_INTERNAL_HANDLE_CONTAINER_
#define WORTHY_INTERNAL_HANDLE_CONTAINER_


#include "internal/atomic_intrusive_stack.h"
#include "internal/block_owner.h"
#include "internal/blocked_vector.h"
#include "internal/handle.h"
#include "internal/owner_block_allocator.h"


namespace worthy {
namespace internal {


class HandleContainer final : public BlockOwner {
public:
    HandleContainer(const HandleContainer&) = delete;
    HandleContainer& operator=(const HandleContainer&) = delete;

    explicit HandleContainer(BlockAllocator* allocator);

    HandlePtr makeHandle(Object* obj);

private:
    void reclaim(Handle* handle);

    OwnerBlockAllocator allocator_;
    BlockedVector<Handle> handles_;

    AtomicIntrusiveStack<Handle> free_handles_;

    friend class HandleAccess;
};


class HandleAccess {
private:
    static void reclaim(Handle* handle);

    friend class Handle;
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_HANDLE_CONTAINER_
