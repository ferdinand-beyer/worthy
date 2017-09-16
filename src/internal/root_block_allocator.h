#ifndef WORTHY_INTERNAL_ROOT_BLOCK_ALLOCATOR_H_
#define WORTHY_INTERNAL_ROOT_BLOCK_ALLOCATOR_H_


#include "internal/block.h"
#include "internal/block_allocator.h"
#include "internal/globals.h"

#include <list>


namespace worthy {
namespace internal {


class RootBlockAllocator final : public BlockAllocator {
public:
    RootBlockAllocator(const RootBlockAllocator&) = delete;
    RootBlockAllocator& operator=(const RootBlockAllocator&) = delete;

    RootBlockAllocator();
    ~RootBlockAllocator() override;

    size_t blocksAllocated() const;
    size_t chunksAllocated() const;

    Block* allocate(size_t block_count = 1) override;

    void deallocate(Block* block) override;

private:
    static constexpr size_t FreeListCount = ChunkBits - BlockBits;

    static Block* nextFreeBlock(Block* block);
    static Block* previousFreeBlock(Block* block);

    static void setupGroup(Block* block, size_t block_count);

    static bool isFree(Block* block);

    void reclaim(Block* block);
    Block* release(Block* block);

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

    size_t blocks_allocated_;
    size_t chunks_allocated_;
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_ROOT_BLOCK_ALLOCATOR_H_
