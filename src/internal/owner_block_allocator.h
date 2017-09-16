#ifndef WORTHY_INTERNAL_OWNER_BLOCK_ALLOCATOR_H_
#define WORTHY_INTERNAL_OWNER_BLOCK_ALLOCATOR_H_


#include "internal/block_allocator.h"


namespace worthy {
namespace internal {


class BlockOwner;


class OwnerBlockAllocator : public BlockAllocator {
public:
    OwnerBlockAllocator(const OwnerBlockAllocator&) = delete;
    OwnerBlockAllocator& operator=(const OwnerBlockAllocator&) = delete;

    OwnerBlockAllocator(BlockOwner* owner, BlockAllocator* allocator);

    Block* allocate(size_t block_count = 1) override;

    void deallocate(Block* block) override;

private:
    BlockOwner* owner_;
    BlockAllocator* allocator_;
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_OWNER_BLOCK_ALLOCATOR_H_

