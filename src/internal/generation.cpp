#include "internal/generation.h"

#include "internal/block_allocator.h"


namespace worthy {
namespace internal {


Generation::Generation(uint16_t generation_no, Heap* heap,
        BlockAllocator* allocator) :
    Space{heap, allocator, generation_no, 0}
{
}


void Generation::deallocateOldBlocks() {
    allocator().deallocateList(old_blocks_);
}


} } // namespace worthy::internal
