#ifndef WORTHY_INTERNAL_HANDLE_POOL_
#define WORTHY_INTERNAL_HANDLE_POOL_


#include "internal/atomic_intrusive_stack.h"
#include "internal/block_owner.h"
#include "internal/blocked_vector.h"
#include "internal/handle.h"
#include "internal/owner_block_allocator.h"


namespace worthy {
namespace internal {


class GCVisitor;


class HandlePool final : public BlockOwner {
public:
    HandlePool(const HandlePool&) = delete;
    HandlePool& operator=(const HandlePool&) = delete;

    explicit HandlePool(BlockAllocator* allocator);

    HandlePtr makeHandle(Object* obj);

    void accept(GCVisitor& visitor);

private:
    void reclaim(Handle* handle);

    OwnerBlockAllocator allocator_;
    BlockedVector<Handle> handles_;

    AtomicIntrusiveStack<Handle> free_handles_;

    friend class HandlePoolAccess;
};


class HandlePoolAccess {
private:
    static void reclaim(Handle* handle);

    friend class Handle;
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_HANDLE_POOL_
