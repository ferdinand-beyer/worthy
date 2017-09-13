#include "internal/block_allocator.h"

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


inline size_t blocksForChunks(size_t chunk_count) {
    WORTHY_DCHECK(chunk_count > 0);
    return BlocksPerChunk + (chunk_count - 1) * (ChunkSize / BlockSize);
}


inline size_t chunksForBlocks(size_t block_count) {
    WORTHY_DCHECK(block_count > 0);
    return 1 + align_up((block_count - BlocksPerChunk) * BlockSize,
            ChunkSize) / ChunkSize;
}


inline byte* chunkAddress(void* ptr) {
    return reinterpret_cast<byte*>(
            reinterpret_cast<uintptr_t>(ptr) & ~ChunkMask);
}


inline Block* chunkBegin(void* ptr) {
    return reinterpret_cast<Block*>(
            chunkAddress(ptr) + BlockDescriptorOffset);
}


inline Block* chunkEnd(void* ptr) {
    return chunkBegin(ptr) + BlocksPerChunk;
}


} // namespace


BlockAllocator::BlockAllocator() :
    chunks_allocated_{0}
{
}


BlockAllocator::~BlockAllocator() {
    for (auto ptr : allocations_) {
        aligned_free(ptr);
    }
}


size_t BlockAllocator::chunksAllocated() const {
    return chunks_allocated_;
}


Block* BlockAllocator::allocate(size_t block_count) {
    WORTHY_CHECK(block_count > 0);

    if (block_count >= BlocksPerChunk) {
        // Might allocate more as requested (whole chunks).
        Block* block = allocateChunkGroup(chunksForBlocks(block_count));
        initBlock(block);
        return block;
    }

    if (Block* block = allocateFromFreeList(block_count)) {
        return block;
    }

    return allocateFromNewChunk(block_count);
}


void BlockAllocator::deallocate(Block* block) {
    WORTHY_CHECK(block && !isFree(block));
    WORTHY_DCHECK(block->block_count_ > 0);

    markFree(block);

    if (block->block_count_ >= BlocksPerChunk) {
        const auto chunk_count = chunksForBlocks(block->block_count_);
        WORTHY_DCHECK(block->block_count_ == blocksForChunks(chunk_count));
        freeChunkGroup(block);
        return;
    }

    if (Block* next = nextFreeBlock(block)) {
        removeFromFreeList(next);
        setupGroup(block, block->block_count_ + next->block_count_);
    }

    if (Block* prev = previousFreeBlock(block)) {
        removeFromFreeList(prev);
        setupGroup(prev, prev->block_count_ + block->block_count_);
        block = prev;
    }

    if (block->block_count_ >= BlocksPerChunk) {
        freeChunkGroup(block);
    } else {
        addToFreeList(block);
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


Block* BlockAllocator::nextFreeBlock(Block* block) {
    Block* next = block + block->block_count_;
    if (next < chunkEnd(block) && isFree(next)) {
        return next;
    }
    return nullptr;
}


Block* BlockAllocator::previousFreeBlock(Block* block) {
    if (block > chunkBegin(block)) {
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


BlockList* BlockAllocator::freeListForAllocation(size_t block_count) {
    size_t index = freeListIndexForAllocation(block_count);
    while (index < FreeListCount && free_blocks_[index].empty()) {
        index++;
    }
    return (index < FreeListCount) ? &free_blocks_[index] : nullptr;
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
    WORTHY_DCHECK(chunk_count > 0);

    const size_t block_count = blocksForChunks(chunk_count);
    Block* best = nullptr;

    for (auto group = free_chunks_.begin(), end = free_chunks_.end();
         group != end; ++group) {
        if (group->block_count_ == block_count) {
            free_chunks_.erase(group);
            return &(*group);
        }
        if ((group->block_count_ > block_count) &&
                (!best || (group->block_count_ < best->block_count_))) {
            best = &(*group);
        }
    }

    byte* chunk_addr;

    if (best) {
        const auto chunks_left =
            chunksForBlocks(best->block_count_) - chunk_count;
        setupGroup(best, blocksForChunks(chunks_left));
        chunk_addr = chunkAddress(best) + chunks_left * ChunkSize;
    } else {
        void* memory = aligned_alloc(ChunkSize, chunk_count * ChunkSize);
        if (!memory) {
            return nullptr; // TODO: Should we throw?
        }
        allocations_.push_front(memory);
        chunks_allocated_ += chunk_count;
        chunk_addr = reinterpret_cast<byte*>(memory);
    }

    initChunkBlocks(chunk_addr);

    Block* group = chunkBegin(chunk_addr);
    setupGroup(group, blocksForChunks(chunk_count));

    return group;
}


void BlockAllocator::initChunkBlocks(byte* chunk_addr) {
    byte* descr_addr = chunk_addr + BlockDescriptorOffset;
    byte* block_addr = chunk_addr + BlockOffset;

    for (uint i = 0; i < BlocksPerChunk; i++) {
        new (descr_addr) Block(block_addr);

        descr_addr += BlockDescriptorSize;
        block_addr += BlockSize;
    }
}


void BlockAllocator::freeChunkGroup(Block* block) {
    WORTHY_DCHECK(block->block_count_ >= BlocksPerChunk);

    // The free_chunks_ list is sorted by address, to identify adjacent chunks
    // that are candidates for merging.
    Block* prev = nullptr;
    auto pos = free_chunks_.begin();
    const auto end = free_chunks_.end();
    while ((pos != end) && (&(*pos) < block)) {
        prev = &(*pos);
        ++pos;
    }

    if (prev && mergeChunkGroups(prev, block)) {
        block = prev;
    } else {
        free_chunks_.insert(pos, *block);
    }

    if ((pos != end) && mergeChunkGroups(block, &(*pos))) {
        free_chunks_.erase(pos);
    }
}


bool BlockAllocator::mergeChunkGroups(Block* block, Block* next) {
    size_t chunk_count = chunksForBlocks(block->block_count_);
    const auto next_chunk = chunkAddress(block) + chunk_count * ChunkSize;
    if (next_chunk == chunkAddress(next)) {
        chunk_count += chunksForBlocks(next->block_count_);
        setupGroup(block, blocksForChunks(chunk_count));
        return true;
    }
    return false;
}


} } // namespace worthy::internal
