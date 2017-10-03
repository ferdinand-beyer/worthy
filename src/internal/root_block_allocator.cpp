#include "internal/root_block_allocator.h"

#include "internal/block_layout.h"
#include "internal/check.h"

#include <boost/align/align_up.hpp>
#include <boost/align/aligned_alloc.hpp>

#include <cstring>
#include <new>


using boost::alignment::align_up;
using boost::alignment::aligned_alloc;
using boost::alignment::aligned_free;


namespace worthy {
namespace internal {


namespace {


inline constexpr size_t blocksForChunks(size_t chunk_count) {
    return BlocksPerChunk + (chunk_count - 1) * (ChunkSize / BlockSize);
}


inline constexpr size_t chunksForBlocks(size_t block_count) {
    return 1 + align_up((block_count - BlocksPerChunk) * BlockSize,
            ChunkSize) / ChunkSize;
}


inline Block* chunkBegin(void* ptr) {
    return reinterpret_cast<Block*>(
            chunkAddress(ptr) + FirstBlockDescriptorOffset);
}


inline Block* chunkEnd(void* ptr) {
    return chunkBegin(ptr) + BlocksPerChunk;
}


} // namespace


RootBlockAllocator::RootBlockAllocator() :
    blocks_allocated_{0},
    chunks_allocated_{0}
{
}


RootBlockAllocator::~RootBlockAllocator() {
    for (auto ptr : allocations_) {
        aligned_free(ptr);
    }
}

size_t RootBlockAllocator::blocksAllocated() const {
    return blocks_allocated_;
}


size_t RootBlockAllocator::chunksAllocated() const {
    return chunks_allocated_;
}


Block* RootBlockAllocator::allocate(size_t block_count) {
    WORTHY_CHECK(block_count > 0);

    if (block_count >= BlocksPerChunk) {
        // Might allocate more as requested (whole chunks).
        Block* block = allocateChunkGroup(chunksForBlocks(block_count));
        return release(block);
    }

    if (Block* block = allocateFromFreeList(block_count)) {
        return block;
    }

    return allocateFromNewChunk(block_count);
}


void RootBlockAllocator::deallocate(Block* block) {
    WORTHY_CHECK(block && !isFree(block));
    WORTHY_DCHECK(block->span_ > 0);

    reclaim(block);

    if (block->span_ >= BlocksPerChunk) {
        const auto chunk_count = chunksForBlocks(block->span_);
        WORTHY_DCHECK(block->span_ == blocksForChunks(chunk_count));
        freeChunkGroup(block);
        return;
    }

    if (Block* next = nextFreeBlock(block)) {
        removeFromFreeList(next);
        setupGroup(block, block->span_ + next->span_);
    }

    if (Block* prev = previousFreeBlock(block)) {
        removeFromFreeList(prev);
        setupGroup(prev, prev->span_ + block->span_);
        block = prev;
    }

    if (block->span_ >= BlocksPerChunk) {
        freeChunkGroup(block);
    } else {
        addToFreeList(block);
    }
}


Block* RootBlockAllocator::nextFreeBlock(Block* block) {
    Block* next = block + block->span_;
    if (next < chunkEnd(block) && isFree(next)) {
        return next;
    }
    return nullptr;
}


Block* RootBlockAllocator::previousFreeBlock(Block* block) {
    if (block > chunkBegin(block)) {
        Block* prev = block - 1;
        if (prev->span_ > 1) {
            // This is the last block in a group, get to the first block
            // of the group.
            prev = block - prev->span_;
        }

        WORTHY_DCHECK(block == (prev + prev->span_));

        if (isFree(prev)) {
            return prev;
        }
    }
    return nullptr;
}


void RootBlockAllocator::setupGroup(Block* block, size_t span) {
    block->span_ = span;
    // Save the count also in the last block of a group, so that we can get
    // back to the head of the group when merging blocks.
    if (span > 1 && span <= BlocksPerChunk) {
        Block* last = block + span - 1;
        last->span_ = span;
    }
}


bool RootBlockAllocator::isFree(Block* block) {
    return block->free_ == nullptr;
}


void RootBlockAllocator::reclaim(Block* block) {
    blocks_allocated_ -= block->span_;
    block->clearState();
    block->free_ = nullptr;

#ifdef WORTHY_DEBUG
    std::memset(block->start_, 0xdd, block->span_ * BlockSize);
#endif
}


Block* RootBlockAllocator::release(Block* block) {
    block->free_ = block->start_;
    blocks_allocated_ += block->span_;

#ifdef WORTHY_DEBUG
    std::memset(block->start_, 0xaa, block->span_ * BlockSize);
#endif

    return block;
}


Block* RootBlockAllocator::allocateFromFreeList(size_t block_count) {
    auto free_list = freeListForAllocation(block_count);
    if (!free_list) {
        // No suitable free block.
        return nullptr;
    }

    Block* block = &free_list->front();
    free_list->pop_front();

    if (block->span_ == block_count) {
        return release(block);
    }

    return allocateFromFreeBlock(block, block_count);
}


Block* RootBlockAllocator::allocateFromFreeBlock(Block* block, size_t block_count) {
    // The block must be free but not on the free list.
    WORTHY_DCHECK(isFree(block));
    WORTHY_DCHECK(block->span_ > block_count);

    // Cut from the end of the free block.
    Block* b = block + block->span_ - block_count;
    setupGroup(b, block_count);

    setupGroup(block, block->span_ - block_count);
    addToFreeList(block);

    return release(b);
}


void RootBlockAllocator::addToFreeList(Block* block) {
    WORTHY_DCHECK(isFree(block));
    const auto index = freeListIndex(block);
    free_blocks_[index].push_front(*block);
}


void RootBlockAllocator::removeFromFreeList(Block* block) {
    WORTHY_DCHECK(isFree(block));
    const auto index = freeListIndex(block);
    auto& list = free_blocks_[index];
    list.erase(list.iterator_to(*block));
}


BlockList* RootBlockAllocator::freeListForAllocation(size_t block_count) {
    size_t index = freeListIndexForAllocation(block_count);
    while (index < FreeListCount && free_blocks_[index].empty()) {
        index++;
    }
    return (index < FreeListCount) ? &free_blocks_[index] : nullptr;
}


size_t RootBlockAllocator::freeListIndex(Block* block) {
    WORTHY_DCHECK(block->span_ > 0 && block->span_ < (1 << FreeListCount));

    // Calculate log2, rounded down.
    size_t n = block->span_;
    for (size_t i = 0; i < FreeListCount; i++) {
        n >>= 1;
        if (n == 0) {
            return i;
        }
    }

    WORTHY_UNREACHABLE();
}


size_t RootBlockAllocator::freeListIndexForAllocation(size_t block_count) {
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


Block* RootBlockAllocator::allocateFromNewChunk(size_t block_count) {
    WORTHY_DCHECK(block_count < BlocksPerChunk);

    Block* b = allocateChunkGroup(1);
    setupGroup(b, block_count);

    Block* rest = b + block_count;
    setupGroup(rest, BlocksPerChunk - block_count);
    addToFreeList(rest);

    return release(b);
}


Block* RootBlockAllocator::allocateChunkGroup(size_t chunk_count) {
    WORTHY_DCHECK(chunk_count > 0);

    const size_t block_count = blocksForChunks(chunk_count);
    Block* best = nullptr;

    for (auto group = free_chunks_.begin(), end = free_chunks_.end();
         group != end; ++group) {
        if (group->span_ == block_count) {
            free_chunks_.erase(group);
            return &(*group);
        }
        if ((group->span_ > block_count) &&
                (!best || (group->span_ < best->span_))) {
            best = &(*group);
        }
    }

    byte* chunk_addr;

    if (best) {
        const auto chunks_left = chunksForBlocks(best->span_) - chunk_count;
        setupGroup(best, blocksForChunks(chunks_left));
        chunk_addr = chunkAddress(best) + chunks_left * ChunkSize;
    } else {
        void* memory = aligned_alloc(ChunkSize, chunk_count * ChunkSize);
        if (!memory) {
            throw std::bad_alloc();
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


void RootBlockAllocator::initChunkBlocks(byte* chunk_addr) {
    byte* descr_addr = chunk_addr + FirstBlockDescriptorOffset;
    byte* block_addr = chunk_addr + FirstBlockOffset;

    for (uint i = 0; i < BlocksPerChunk; i++) {
        new (descr_addr) Block(block_addr);

        descr_addr += BlockDescriptorSize;
        block_addr += BlockSize;
    }
}


void RootBlockAllocator::freeChunkGroup(Block* block) {
    WORTHY_DCHECK(block->span_ >= BlocksPerChunk);

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


bool RootBlockAllocator::mergeChunkGroups(Block* block, Block* next) {
    size_t chunk_count = chunksForBlocks(block->span_);
    const auto next_chunk = chunkAddress(block) + chunk_count * ChunkSize;
    if (next_chunk == chunkAddress(next)) {
        chunk_count += chunksForBlocks(next->span_);
        setupGroup(block, blocksForChunks(chunk_count));
        return true;
    }
    return false;
}


} } // namespace worthy::internal
