#include "internal/block_allocator.h"


namespace worthy {
namespace internal {


void BlockAllocator::deallocateList(BlockList& blocks) {
    auto block = blocks.begin();
    auto end = blocks.end();
    while (block != end) {
        block = blocks.erase_and_dispose(
                block, [&](auto b) { deallocate(b); });
    }
}


} } // namespace worthy::internal
