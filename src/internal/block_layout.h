#ifndef WORTHY_INTERNAL_BLOCK_LAYOUT_H_
#define WORTHY_INTERNAL_BLOCK_LAYOUT_H_


#include "internal/block.h"


namespace worthy {
namespace internal {


static constexpr uintptr_t ChunkMask = ChunkSize - 1;
static constexpr uintptr_t BlockMask = BlockSize - 1;

static constexpr size_t BlocksPerChunk =
    ChunkSize / (BlockSize + BlockDescriptorSize);

static constexpr size_t FirstBlockOffset =
    ChunkSize - (BlockSize * BlocksPerChunk);


// Scale the offset of a block within a chunk to the offset of its descriptor.
inline constexpr size_t descriptorFromBlockOffset(size_t block_offset) {
    return block_offset >> (BlockBits - BlockDescriptorBits);
}


inline constexpr size_t blockFromDescriptorOffset(size_t descr_offset) {
    return descr_offset << (BlockBits - BlockDescriptorBits);
}


static constexpr size_t FirstBlockDescriptorOffset =
    descriptorFromBlockOffset(FirstBlockOffset);


inline constexpr uintptr_t blockFromDescriptor(uintptr_t descr) {
    return (descr & ~ChunkMask) | blockFromDescriptorOffset(descr & ChunkMask);
}


inline byte* blockFromDescriptor(byte* descr) {
    return reinterpret_cast<byte*>(
            blockFromDescriptor(reinterpret_cast<uintptr_t>(descr)));
}


inline byte* chunkAddress(void* ptr) {
    return reinterpret_cast<byte*>(
            reinterpret_cast<uintptr_t>(ptr) & ~ChunkMask);
}


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_BLOCK_LAYOUT_H_
