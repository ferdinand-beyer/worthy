#include "internal/block-allocator.h"

#include "internal/check.h"

#include <boost/align/align_up.hpp>
#include <boost/align/aligned_alloc.hpp>

#include <new>


using boost::alignment::align_up;
using boost::alignment::aligned_alloc;
using boost::alignment::aligned_free;


namespace worthy {
namespace internal {


namespace {


inline size_t chunksForBlocks(size_t block_count) {
    return 1 + align_up((block_count - BlocksPerChunk) * BlockSize, ChunkSize)
        / ChunkSize;
}


inline Block* chunkStart(Block* block) {
    return reinterpret_cast<Block*>(
            (reinterpret_cast<uintptr_t>(block) & ~ChunkMask)
            + BlockDescriptorOffset);
}


inline Block* chunkEnd(Block* block) {
    return chunkStart(block) + BlocksPerChunk;
}


} // namespace


BlockAllocator::BlockAllocator() {
}


BlockAllocator::~BlockAllocator() {
    for (auto ptr : allocated_chunks_) {
        aligned_free(ptr);
    }
}


size_t BlockAllocator::chunksAllocated() const {
    return allocated_chunks_.size();
}


Block* BlockAllocator::allocateBlock() {
    return allocateBlockGroup(1);
}


Block* BlockAllocator::allocateBlockGroup(size_t block_count) {
    WORTHY_CHECK(block_count > 0);

    if (block_count >= BlocksPerChunk) {
        Block* b = allocateChunkGroup(chunksForBlocks(block_count));
        setupGroup(b, block_count);
        initBlock(b);
        return b;
    }

    if (Block* b = allocateFromFreeList(block_count)) {
        return b;
    }

    return allocateFromNewChunk(block_count);
}


void BlockAllocator::deallocate(Block* block) {
    WORTHY_CHECK(block && !isFree(block));

    markFree(block);

    WORTHY_DCHECK(block->block_count_ > 0);

    if (block->block_count_ >= BlocksPerChunk) {
        // TODO
        WORTHY_UNIMPLEMENTED();
    }

    // Merge with next free block.
    if (Block* next = nextFreeBlock(block)) {
        removeFromFreeList(next);
        const auto count = block->block_count_ + next->block_count_;
        if (count == BlocksPerChunk) {
            // TODO: Free chunk
            WORTHY_UNIMPLEMENTED();
        }
        setupGroup(block, count);
    }

    // Merge with previous free block.
    if (Block* prev = previousFreeBlock(block)) {
        removeFromFreeList(prev);
        const auto count = prev->block_count_ + block->block_count_;
        if (count >= BlocksPerChunk) {
            // TODO: Free chunk
            WORTHY_UNIMPLEMENTED();
        }
        block = prev;
        setupGroup(block, count);
    }

    addToFreeList(block);
}


size_t BlockAllocator::freeListIndex(Block* block) {
    const auto block_count = block->block_count_;
    WORTHY_DCHECK(block_count > 0 && block_count < (1 << FreeListCount));

    // Calculate log2, rounded down.
    size_t n = block_count;
    for (size_t i = 0; i < FreeListCount; i++) {
        n >>= 1;
        if (n == 0) {
            return i;
        }
    }

    WORTHY_UNREACHABLE();
}


size_t BlockAllocator::freeListIndexForAllocation(size_t block_count) {
    WORTHY_DCHECK(block_count > 0 && block_count < (1 << FreeListCount));

    // Calculate log2, rounded up.
    size_t n = 1;
    for (size_t i = 0; i < FreeListCount; i++) {
        if (n >= block_count) {
            return i;
        }
        n <<= 1;
    }
    return FreeListCount;
}


Block* BlockAllocator::nextFreeBlock(Block* block) {
    Block* next = block + block->block_count_;
    if (next < chunkEnd(block) && isFree(next)) {
        return next;
    }
    return nullptr;
}


Block* BlockAllocator::previousFreeBlock(Block* block) {
    if (block > chunkStart(block)) {
        Block* prev = block - 1;
        if (prev->block_count_ > 1) {
            // This is the last block in a group, get to the first block
            // of the group.
            prev = block - prev->block_count_;
        }

        WORTHY_DCHECK(block == (prev + prev->block_count_));

        if (isFree(prev)) {
            return prev;
        }
    }
    return nullptr;
}


void BlockAllocator::setupGroup(Block* block, size_t block_count) {
    block->block_count_ = block_count;
    // Save the count also in the last block of a group, so that we can get
    // back to the head of the group when merging blocks.
    if (block_count > 1 && block_count <= BlocksPerChunk) {
        Block* last = block + block_count - 1;
        last->block_count_ = block_count;
    }
}


bool BlockAllocator::isFree(Block* block) {
    return block->free_ == nullptr;
}


void BlockAllocator::markFree(Block* block) {
    block->free_ = nullptr;
}


void BlockAllocator::initBlock(Block* block) {
    block->free_ = block->start_;
}


Block* BlockAllocator::allocateFromFreeList(size_t block_count) {
    auto free_list = freeListForAllocation(block_count);
    if (!free_list) {
        // No suitable free block.
        return nullptr;
    }

    Block* block = &free_list->front();
    free_list->pop_front();

    if (block->block_count_ == block_count) {
        initBlock(block);
        return block;
    }

    return allocateFromFreeBlock(block, block_count);
}


Block* BlockAllocator::allocateFromFreeBlock(Block* block, size_t block_count) {
    // The block must be free but not on the free list.
    WORTHY_DCHECK(isFree(block));
    WORTHY_DCHECK(block->block_count_ > block_count);

    // Cut from the end of the free block.
    Block* b = block + block->block_count_ - block_count;
    setupGroup(b, block_count);

    setupGroup(block, block->block_count_ - block_count);
    addToFreeList(block);

    initBlock(b);
    return b;
}


BlockList* BlockAllocator::freeListForAllocation(size_t block_count) {
    size_t index = freeListIndexForAllocation(block_count);
    while (index < FreeListCount && free_blocks_[index].empty()) {
        index++;
    }
    return (index < FreeListCount) ? &free_blocks_[index] : nullptr;
}


void BlockAllocator::addToFreeList(Block* block) {
    WORTHY_DCHECK(isFree(block));
    const auto index = freeListIndex(block);
    free_blocks_[index].push_front(*block);
}


void BlockAllocator::removeFromFreeList(Block* block) {
    WORTHY_DCHECK(isFree(block));
    const auto index = freeListIndex(block);
    auto& list = free_blocks_[index];
    list.erase(list.iterator_to(*block));
}


Block* BlockAllocator::allocateFromNewChunk(size_t block_count) {
    WORTHY_DCHECK(block_count < BlocksPerChunk);

    Block* b = allocateChunkGroup(1);
    if (!b) {
        return nullptr; // TODO: Should allocateChunkGroup() throw?
    }
    setupGroup(b, block_count);

    Block* rest = b + block_count;
    setupGroup(rest, BlocksPerChunk - block_count);
    addToFreeList(rest);

    initBlock(b);
    return b;
}


Block* BlockAllocator::allocateChunkGroup(size_t chunk_count) {
    // TODO: For later...
    if (chunk_count != 1) {
        WORTHY_UNIMPLEMENTED();
    }

    void* chunk = aligned_alloc(ChunkSize, ChunkSize);
    if (!chunk) {
        return nullptr; // TODO: Should we throw?
    }

    allocated_chunks_.push_front(chunk);

    byte* const chunk_addr = reinterpret_cast<byte*>(chunk);

    byte* descr_addr = chunk_addr + BlockDescriptorOffset;
    byte* block_addr = chunk_addr + BlockOffset;

    Block* const first_descr = reinterpret_cast<Block*>(descr_addr);

    // Initialize all block descriptors.
    for (uint i = 0; i < BlocksPerChunk; i++) {
        new (descr_addr) Block(block_addr);

        descr_addr += BlockDescriptorSize;
        block_addr += BlockSize;
    }

    return first_descr;
}


} } // namespace worthy::internal
