#include "internal/generation.h"


namespace worthy {
namespace internal {


Generation::Generation(uint16_t number, Heap* heap, BlockAllocator* allocator)
    : Space{heap, allocator, number} {
}


} } // namespace worthy::internal
