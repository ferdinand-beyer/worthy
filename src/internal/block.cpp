#include "internal/block.h"

#include "internal/block_layout.h"
#include "internal/check.h"

#include <new>


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
    owner_{nullptr},
    block_count_{0}
{
    WORTHY_DCHECK(start);
}


byte* Block::begin() const {
    return start_;
}


byte* Block::current() const {
    return free_;
}


byte* Block::end() const {
    return start_ + capacity();
}


size_t Block::capacity() const {
    return block_count_ * BlockSize;
}


size_t Block::bytesAllocated() const {
    return free_ - start_;
}


size_t Block::bytesAvailable() const {
    return end() - free_;
}


void* Block::allocate(size_t size) {
    WORTHY_CHECK(size > 0);
    byte* new_free = free_ + size;
    if (new_free > end()) {
        return nullptr;
    }
    byte* result = free_;
    free_ = new_free;
    return result;
}


void Block::deallocate(size_t size) {
    WORTHY_CHECK(size > 0 && size <= bytesAllocated());
    free_ -= size;
}


void BlockOwnerAccess::take(BlockOwner* owner, Block* block) {
    WORTHY_DCHECK(owner && block && !block->owner_);
    block->owner_ = owner;
}


Block* BlockTestAccess::construct(byte* buffer) {
    byte* chunk_addr = chunkAddress(buffer + ChunkSize);
    byte* descr_addr = chunk_addr + FirstBlockDescriptorOffset;
    byte* block_addr = chunk_addr + FirstBlockOffset;

    Block* block = ::new (descr_addr) Block(block_addr);
    block->free_ = block_addr;
    block->block_count_ = 1;

    return block;
}


} } // namespace worthy::internal
