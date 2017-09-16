#ifndef WORTHY_INTERNAL_BLOCK_ALLOCATOR_H_
#define WORTHY_INTERNAL_BLOCK_ALLOCATOR_H_


#include "internal/block.h"
#include "internal/globals.h"


namespace worthy {
namespace internal {


class BlockAllocator {
public:
    virtual ~BlockAllocator() = default;

    virtual Block* allocate(size_t block_count = 1) = 0;

    virtual void deallocate(Block* block) = 0;

    virtual void deallocateList(BlockList& blocks);
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_BLOCK_ALLOCATOR_H_
