#ifndef WORTHY_INTERNAL_BLOCK_CONSTANTS_H_
#define WORTHY_INTERNAL_BLOCK_CONSTANTS_H_


#include "internal/globals.h"


namespace worthy {
namespace internal {


static constexpr size_t BlockBits = 12;
static constexpr size_t ChunkBits = 20;
static constexpr size_t DescriptorBits = 6;

static constexpr size_t BlockSize = 1 << BlockBits;
static constexpr size_t ChunkSize = 1 << ChunkBits;
static constexpr size_t DescriptorSize = 1 << DescriptorBits;

static constexpr uintptr_t BlockMask = BlockSize - 1;
static constexpr uintptr_t ChunkMask = ChunkSize - 1;

static constexpr size_t BlocksPerChunk =
    ChunkSize / (BlockSize + DescriptorSize);

static constexpr size_t BlockOffset =
    ChunkSize - (BlockSize * BlocksPerChunk);

static constexpr size_t DescriptorOffset =
    BlockOffset >> (BlockBits - DescriptorBits);


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_BLOCK_CONSTANTS_H_