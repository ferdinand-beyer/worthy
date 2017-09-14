#include "internal/block.h"

#include "internal/block_layout.h"
#include "internal/check.h"


namespace worthy {
namespace internal {


namespace {


inline constexpr uintptr_t descriptorOffset(uintptr_t offset) {
    return (offset & ~ChunkMask) |
        descriptorFromBlockOffset(offset & ChunkMask & ~BlockMask);

}


} // namespace


Block* Block::of(void* ptr) {
    return reinterpret_cast<Block*>(
            descriptorOffset(reinterpret_cast<uintptr_t>(ptr)));
}


Block::Block(byte* start) :
    start_{start},
    free_{nullptr},
    block_count_{0}
{
    WORTHY_DCHECK(start);
}


byte* Block::begin() const {
    return start_;
}


byte* Block::end() const {
    return start_ + (block_count_ * BlockSize);
}


size_t Block::bytesAvailable() const {
    return end() - free_;
}


} } // namespace worthy::internal
