#ifndef WORTHY_INTERNAL_BLOCK_ALLOCATOR_H_
#define WORTHY_INTERNAL_BLOCK_ALLOCATOR_H_


#include "internal/block.h"
#include "internal/block-constants.h"
#include "internal/globals.h"


namespace worthy {
namespace internal {


class Block;


class BlockAllocator {
public:
    Block* allocateBlock();

private:
    static constexpr size_t FreeListCount = ChunkBits - BlockBits;

    Block* allocateChunk();

    BlockList free_blocks_[FreeListCount];
    BlockList free_chunks_;
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_BLOCK_ALLOCATOR_H_
