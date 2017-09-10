#include "internal/block-allocator.h"

#include <boost/align/aligned_alloc.hpp>

#include <new>


using boost::alignment::aligned_alloc;
using boost::alignment::aligned_free;


namespace worthy {
namespace internal {


namespace {


uint log2(uint n, uint limit) {
    uint x = n;
    for (uint i = 0; i < limit; ++i) {
        x = x >> 1;
        if (x == 0) {
            return i;
        }
    }
    return limit;
}


uint log2_ceil(uint n, uint limit) {
    uint x = 1;
    for (uint i = 0; i < limit; ++i) {
        if (x >= n) {
            return i;
        }
        x = x << 1;
    }
    return limit;
}


} // namespace


Block* BlockAllocator::allocateBlock() {
    // TODO: Look at free list first.
    return allocateChunk();
}


Block* BlockAllocator::allocateChunk() {
    void* memory = aligned_alloc(ChunkSize, ChunkSize);

    if (!memory) {
        // TODO: Exception?
        return nullptr;
    }

    // TODO: Remember (owned!) memory so that we can free it later!

    byte* const chunk_addr = reinterpret_cast<byte*>(memory);

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
