#ifndef WORTHY_INTERNAL_BLOCK_ALLOCATOR_H_
#define WORTHY_INTERNAL_BLOCK_ALLOCATOR_H_


#include "internal/block.h"
#include "internal/block-constants.h"
#include "internal/globals.h"

#include <list>


namespace worthy {
namespace internal {


class Block;


class BlockAllocator final {
public:
    BlockAllocator();
    ~BlockAllocator();

    /**
     * Return the number of chunks allocated from the OS.
     */
    size_t chunksAllocated() const;

    Block* allocateBlock();

    void deallocate(Block* block);

private:
    static constexpr size_t FreeListCount = ChunkBits - BlockBits;

    Block* allocateBlockGroup(size_t block_count);

    Block* allocateFromFreeList(size_t block_count);
    Block* splitFreeBlock(Block* block, size_t block_count);
    void addToFreeList(Block* block);

    size_t sourceFreeListIndex(size_t block_count);
    size_t targetFreeListIndex(size_t block_count);

    Block* allocateFromFreshChunk(size_t block_count);
    Block* allocateChunkGroup(size_t chunk_count);

    std::list<void*> allocated_chunks_;

    BlockList free_blocks_[FreeListCount];
    BlockList free_chunks_;
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_BLOCK_ALLOCATOR_H_
