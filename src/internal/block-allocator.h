#ifndef WORTHY_INTERNAL_BLOCK_ALLOCATOR_H_
#define WORTHY_INTERNAL_BLOCK_ALLOCATOR_H_


#include "internal/block.h"
#include "internal/block-layout.h"
#include "internal/globals.h"

#include <list>


namespace worthy {
namespace internal {


class BlockAllocator final {
public:
    BlockAllocator();
    ~BlockAllocator();

    /**
     * Return the number of chunks allocated from the OS.
     */
    size_t chunksAllocated() const;

    Block* allocateBlock();
    Block* allocateBlockGroup(size_t block_count);

    void deallocate(Block* block);

private:
    WORTHY_DISABLE_COPY(BlockAllocator);

    static constexpr size_t FreeListCount = ChunkBits - BlockBits;

    static Block* nextFreeBlock(Block* block);
    static Block* previousFreeBlock(Block* block);

    static void setupGroup(Block* block, size_t block_count);

    static size_t freeListIndex(Block* block);
    static size_t freeListIndexForAllocation(size_t block_count);

    Block* allocateFromFreeList(size_t block_count);
    Block* allocateFromFreeBlock(Block* block, size_t block_count);

    BlockList* freeListForAllocation(size_t block_count);

    void addToFreeList(Block* block);
    void removeFromFreeList(Block* block);

    Block* allocateFromNewChunk(size_t block_count);
    Block* allocateChunkGroup(size_t chunk_count);

    std::list<void*> allocated_chunks_;

    BlockList free_blocks_[FreeListCount];
    //BlockList free_chunks_;
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_BLOCK_ALLOCATOR_H_
