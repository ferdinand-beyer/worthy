#include "internal/block.h"

#include "internal/block-constants.h"
#include "internal/check.h"


namespace worthy {
namespace internal {


Block* Block::of(void* ptr) {
    const uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
    return reinterpret_cast<Block*>(
            ((addr & ChunkMask & ~BlockMask) >> (BlockBits - DescriptorBits))
            | (addr & ~ChunkMask));
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


bool Block::isFree() const {
    return free_ == nullptr;
}


void Block::setFree() {
    free_ = nullptr;
}


void Block::init() {
    free_ = start_;
}


} } // namespace worthy::internal
