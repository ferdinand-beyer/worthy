#include "internal/block.h"

#include "internal/block-constants.h"
#include "internal/check.h"


namespace worthy {
namespace internal {


static_assert(sizeof(Block) <= DescriptorSize,
        "not enough bits for block descriptor");


Block::Block(byte* start) : start_{start} {
    WORTHY_DCHECK(start);
}


Block* Block::of(void* ptr) {
    const uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
    return reinterpret_cast<Block*>(
            ((addr & ChunkMask & ~BlockMask) >> (BlockBits - DescriptorBits))
            | (addr & ~ChunkMask));
}


} } // namespace worthy::internal
