#include "internal/generation.h"


namespace worthy {
namespace internal {


Generation::Generation(size_t index, Heap* heap, BlockAllocator* allocator)
    : Space{heap, allocator},
      index_{index} {
}


size_t Generation::index() const {
    return index_;
}


} } // namespace worthy::internal
