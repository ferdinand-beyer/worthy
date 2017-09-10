#include "internal/block-allocator.h"

#include "internal/check.h"

#include <boost/align/aligned_alloc.hpp>

#include <new>


using boost::alignment::aligned_alloc;
using boost::alignment::aligned_free;


namespace worthy {
namespace internal {


namespace {


inline Block* chunkStart(Block* block) {
    return reinterpret_cast<Block*>(
            (reinterpret_cast<uintptr_t>(block) & ~ChunkMask)
            + DescriptorOffset);
}


inline Block* chunkEnd(Block* block) {
    return chunkStart(block) + BlocksPerChunk;
}


} // namespace


size_t BlockAllocator::freeListIndex(Block* block) {
    // Correct free list to place/locate blocks.
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
    return FreeListCount;
}


size_t BlockAllocator::freeListIndex(size_t block_count) {
    // Correct free list to look for large enough blocks.
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


void BlockAllocator::setupGroup(Block* block, size_t block_count) {
    block->block_count_ = block_count;
    // Save the count also in the last block of a group, so that we can get
    // back to the head of the group when merging blocks.
    if (block_count > 1 && block_count <= BlocksPerChunk) {
        Block* last = block + block_count - 1;
        last->block_count_ = block_count;
    }
}


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
        // TODO: Fresh chunk(s)
        WORTHY_UNIMPLEMENTED();
    }

    if (Block* b = allocateFromFreeList(block_count)) {
        return b;
    }

    return allocateFromFreshChunk(block_count);
}


void BlockAllocator::deallocate(Block* block) {
    WORTHY_CHECK(block && !block->isFree());

    block->setFree();

    WORTHY_DCHECK(block->block_count_ > 0);

    if (block->block_count_ >= BlocksPerChunk) {
        // TODO
        WORTHY_UNIMPLEMENTED();
    }

    Block* next = block + block->block_count_;

    if (next < chunkEnd(block) && next->isFree()) {
        removeFromFreeList(next);
        const auto count = block->block_count_ + next->block_count_;
        if (count == BlocksPerChunk) {
            // TODO: Free chunk
            WORTHY_UNIMPLEMENTED();
        }
        setupGroup(block, count);
    }

    if (block > chunkStart(block)) {
        Block* prev = block - 1;
        if (prev->block_count_ > 1) {
            // This is the last block in a group.
            prev = block - prev->block_count_;
        }

        WORTHY_DCHECK(block == (prev + prev->block_count_));

        if (prev->isFree()) {
            removeFromFreeList(prev);
            const auto count = prev->block_count_ + block->block_count_;
            if (count == BlocksPerChunk) {
                // TODO: Free chunk
                WORTHY_UNIMPLEMENTED();
            }
            block = prev;
            setupGroup(block, count);
        }
    }

    addToFreeList(block);
}


Block* BlockAllocator::allocateFromFreeList(size_t block_count) {
    size_t index = freeListIndex(block_count);
    while (index < FreeListCount && free_blocks_[index].empty()) {
        index++;
    }

    if (index == FreeListCount) {
        return nullptr;
    }

    WORTHY_DCHECK(index < FreeListCount);

    Block* b = &free_blocks_[index].front();
    free_blocks_[index].pop_front();

    if (b->block_count_ != block_count) {
        b = splitFreeBlock(b, block_count);
    }

    b->init();
    return b;
}


Block* BlockAllocator::splitFreeBlock(Block* block, size_t block_count) {
    // The block must be free but not on the free list.
    WORTHY_DCHECK(block->isFree());
    WORTHY_DCHECK(block->block_count_ > block_count);

    // Cut from the end of the free block.
    Block* b = block + block->block_count_ - block_count;
    setupGroup(b, block_count);

    setupGroup(block, block->block_count_ - block_count);
    addToFreeList(block);

    return b;
}


void BlockAllocator::addToFreeList(Block* block) {
    WORTHY_DCHECK(block->isFree());
    const auto index = freeListIndex(block);
    WORTHY_DCHECK(index < FreeListCount);
    free_blocks_[index].push_front(*block);
}


void BlockAllocator::removeFromFreeList(Block* block) {
    WORTHY_DCHECK(block->isFree());
    const auto index = freeListIndex(block);
    WORTHY_DCHECK(index < FreeListCount);
    auto& list = free_blocks_[index];
    list.erase(list.iterator_to(*block));
}


Block* BlockAllocator::allocateFromFreshChunk(size_t block_count) {
    WORTHY_DCHECK(block_count < BlocksPerChunk);

    Block* b = allocateChunkGroup(1);
    setupGroup(b, block_count);

    Block* rest = b + block_count;
    setupGroup(rest, BlocksPerChunk - block_count);
    addToFreeList(rest);

    b->init();
    return b;
}


Block* BlockAllocator::allocateChunkGroup(size_t chunk_count) {
    // TODO: For later...
    if (chunk_count != 1) {
        WORTHY_UNIMPLEMENTED();
    }

    void* chunk = aligned_alloc(ChunkSize, ChunkSize);

    if (!chunk) {
        // TODO: Exception?
        return nullptr;
    }

    allocated_chunks_.push_front(chunk);

    byte* const chunk_addr = reinterpret_cast<byte*>(chunk);

    byte* descr_addr = chunk_addr + DescriptorOffset;
    byte* block_addr = chunk_addr + BlockOffset;

    Block* const first_descr = reinterpret_cast<Block*>(descr_addr);

    // Initialize all block descriptors.
    for (uint i = 0; i < BlocksPerChunk; i++) {
        new (descr_addr) Block(block_addr);

        descr_addr += DescriptorSize;
        block_addr += BlockSize;
    }

    return first_descr;
}


} } // namespace worthy::internal
