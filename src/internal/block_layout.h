#ifndef WORTHY_INTERNAL_BLOCK_LAYOUT_H_
#define WORTHY_INTERNAL_BLOCK_LAYOUT_H_


#include "internal/globals.h"


namespace worthy {
namespace internal {


static constexpr size_t ChunkBits = 20;
static constexpr size_t BlockBits = 12;
static constexpr size_t BlockDescriptorBits = 6;

static constexpr size_t ChunkSize = 1 << ChunkBits;
static constexpr size_t BlockSize = 1 << BlockBits;
static constexpr size_t BlockDescriptorSize = 1 << BlockDescriptorBits;

static constexpr uintptr_t ChunkMask = ChunkSize - 1;
static constexpr uintptr_t BlockMask = BlockSize - 1;

static constexpr size_t BlocksPerChunk =
    ChunkSize / (BlockSize + BlockDescriptorSize);

static constexpr size_t BlockOffset =
    ChunkSize - (BlockSize * BlocksPerChunk);


// Scale the offset of a block within a chunk to the offset of its descriptor.
inline constexpr size_t descriptorFromBlockOffset(size_t block_offset) {
    return block_offset >> (BlockBits - BlockDescriptorBits);
}


static constexpr size_t BlockDescriptorOffset =
    descriptorFromBlockOffset(BlockOffset);



} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_BLOCK_LAYOUT_H_
