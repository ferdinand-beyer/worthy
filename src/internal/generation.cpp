#include "internal/generation.h"

#include "internal/block_allocator.h"


namespace worthy {
namespace internal {


Generation::Generation(uint16_t generation_no, Heap* heap,
        BlockAllocator* allocator)
    : Space{heap, allocator},
      generation_no_{generation_no} {
}


uint16_t Generation::generationNumber() const {
    return generation_no_;
}


void Generation::initBlock(Block& block) const {
    block.setGenerationNumber(generation_no_);
}


void Generation::deallocateOldBlocks() {
    allocator().deallocateList(old_blocks_);
}


} } // namespace worthy::internal
