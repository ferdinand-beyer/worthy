#include "internal/block.h"

#include "internal/block_layout.h"
#include "internal/check.h"

#include <memory>


namespace worthy {
namespace internal {


namespace {


inline constexpr uintptr_t descriptorOffset(uintptr_t offset) {
    return (offset & ~ChunkMask) |
        descriptorFromBlockOffset(offset & ChunkMask & ~BlockMask);

}


} // namespace


size_t blocksForBytes(size_t size) {
    size_t count = size / BlockSize;
    if (size > (count * BlockSize)) {
        ++count;
    }
    return count;
}


Block* Block::of(void* ptr) {
    return reinterpret_cast<Block*>(
            descriptorOffset(reinterpret_cast<uintptr_t>(ptr)));
}


Block::Block(byte* start) :
    start_{start},
    free_{nullptr},
    owner_{nullptr},
    span_{0},
    flags_{0},
    generation_no_{0},
    next_generation_no_{0}
{
    WORTHY_DCHECK(start);
}


void Block::clearMetaData() {
    owner_ = nullptr;
    flags_ = 0;
    generation_no_ = 0;
    next_generation_no_ = 0;
}


BlockOwner* Block::owner() const {
    return owner_;
}


void Block::setOwner(BlockOwner* owner) {
    owner_ = owner;
}


uint16_t Block::generationNumber() const {
    return generation_no_;
}


void Block::setGenerationNumber(uint16_t generation_no) {
    generation_no_ = generation_no;
}


uint16_t Block::nextGenerationNumber() const {
    return next_generation_no_;
}


void Block::setNextGenerationNumber(uint16_t generation_no) {
    next_generation_no_ = generation_no;
}


uint16_t Block::flags() const {
    return flags_;
}


uint16_t& Block::flags() {
    return flags_;
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
    return span_ * BlockSize;
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


void* Block::allocate(size_t size, size_t alignment) {
    WORTHY_CHECK(size > 0);

    void* ptr = free_;
    size_t space = bytesAvailable();

    if (std::align(alignment, size, ptr, space)) {
        free_ = reinterpret_cast<byte*>(ptr) + size;
        return ptr;
    }

    return nullptr;
}


void Block::deallocate(size_t size) {
    WORTHY_CHECK(size > 0 && size <= bytesAllocated());
    free_ -= size;
}


Block* BlockTestAccess::construct(byte* buffer) {
    byte* chunk_addr = chunkAddress(buffer + ChunkSize);
    byte* descr_addr = chunk_addr + FirstBlockDescriptorOffset;
    byte* block_addr = chunk_addr + FirstBlockOffset;

    Block* block = ::new (descr_addr) Block(block_addr);
    block->free_ = block_addr;
    block->span_ = 1;

    return block;
}


} } // namespace worthy::internal
