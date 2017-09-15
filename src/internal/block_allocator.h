#ifndef WORTHY_INTERNAL_BLOCK_ALLOCATOR_H_
#define WORTHY_INTERNAL_BLOCK_ALLOCATOR_H_


#include "internal/block.h"
#include "internal/globals.h"

#include <list>


namespace worthy {
namespace internal {


class BlockAllocator final {
public:
    BlockAllocator();
    ~BlockAllocator();

    WORTHY_DISABLE_COPY(BlockAllocator);

    /**
     * Return the number of chunks allocated from the OS.
     */
    size_t chunksAllocated() const;

    /**
     * Allocate a group of block_count blocks.
     */
    Block* allocate(size_t block_count = 1);

    /**
     * Deallocate a block group.
     */
    void deallocate(Block* block);

    void deallocate(BlockList& blocks);

private:
    static constexpr size_t FreeListCount = ChunkBits - BlockBits;

    static bool isFree(Block* block);
    static void markFree(Block* block);
    static void markInUse(Block* block);

    static Block* nextFreeBlock(Block* block);
    static Block* previousFreeBlock(Block* block);

    static void setupGroup(Block* block, size_t block_count);

    Block* allocateFromFreeList(size_t block_count);
    Block* allocateFromFreeBlock(Block* block, size_t block_count);

    void addToFreeList(Block* block);
    void removeFromFreeList(Block* block);

    BlockList* freeListForAllocation(size_t block_count);

    static size_t freeListIndex(Block* block);
    static size_t freeListIndexForAllocation(size_t block_count);

    Block* allocateFromNewChunk(size_t block_count);
    Block* allocateChunkGroup(size_t chunk_count);

    static void initChunkBlocks(byte* chunk_addr);

    void freeChunkGroup(Block* block);

    static bool mergeChunkGroups(Block* block, Block* next);

    BlockList free_blocks_[FreeListCount];
    BlockList free_chunks_;

    std::list<void*> allocations_;

    size_t chunks_allocated_;
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_BLOCK_ALLOCATOR_H_
