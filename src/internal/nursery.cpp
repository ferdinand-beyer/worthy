#include "internal/nursery.h"

#include "internal/block.h"


namespace worthy {
namespace internal {


Nursery::Nursery(Heap* heap, BlockAllocator* allocator)
    : Space(heap, allocator) {
}


void Nursery::initBlock(Block& block) const {
    block.setGenerationNumber(0);
}


} } // namespace worthy::internal
