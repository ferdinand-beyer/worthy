#include "internal/block-allocator.h"

#include "internal/check.h"

#include <boost/align/aligned_alloc.hpp>

#include <new>


using boost::alignment::aligned_alloc;
using boost::alignment::aligned_free;


namespace worthy {
namespace internal {


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


void BlockAllocator::deallocate(Block* block) {
    WORTHY_CHECK(block);
    WORTHY_UNIMPLEMENTED();
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


Block* BlockAllocator::allocateFromFreeList(size_t block_count) {
    size_t index = sourceFreeListIndex(block_count);
    while (index < FreeListCount && free_blocks_[index].empty()) {
        index++;
    }

    if (index == FreeListCount) {
        return nullptr;
    }

    WORTHY_DCHECK(index < FreeListCount);

    Block* b = &free_blocks_[index].front();
    free_blocks_[index].pop_front();

    if (b->blockCount() != block_count) {
        b = splitFreeBlock(b, block_count);
    }

    b->init(block_count); // TODO: block_count already set
    return b;
}


Block* BlockAllocator::splitFreeBlock(Block* block, size_t block_count) {
    WORTHY_DCHECK(block->blockCount() > block_count);

    Block* b = block + block->blockCount() - block_count;
    b->block_count_ = block_count; // TODO: Private...

    block->block_count_ -= block_count; // TODO: Private...
    addToFreeList(block);

    return b;
}


void BlockAllocator::addToFreeList(Block* block) {
    WORTHY_DCHECK(block->isFree());
    const auto index = targetFreeListIndex(block->blockCount());
    WORTHY_DCHECK(index < FreeListCount);
    free_blocks_[index].push_back(*block);
}


size_t BlockAllocator::sourceFreeListIndex(size_t block_count) {
    // Calculate (ceil) log2.
    size_t n = 1;
    for (size_t i = 0; i < FreeListCount; ++i) {
        if (n >= block_count) {
            return i;
        }
        n <<= 1;
    }
    return FreeListCount;
}


size_t BlockAllocator::targetFreeListIndex(size_t block_count) {
    // Calculate (floor) log2.
    size_t n = block_count;
    for (size_t i = 0; i < FreeListCount; i++) {
        n >>= 1;
        if (n == 0) {
            return i;
        }
    }
    return FreeListCount;
}


Block* BlockAllocator::allocateFromFreshChunk(size_t block_count) {
    WORTHY_DCHECK(block_count < BlocksPerChunk);

    Block* b = allocateChunkGroup(1);
    b->init(block_count);

    Block* rest = b + block_count;
    rest->block_count_ = BlocksPerChunk - block_count; // TODO: Private...
    addToFreeList(rest);

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

    // Initialize all blocks.
    for (uint i = 0; i < BlocksPerChunk; ++i) {
        new (descr_addr) Block(block_addr);
        descr_addr += DescriptorSize;
        block_addr += BlockSize;
    }

    return first_descr;
}


} } // namespace worthy::internal
