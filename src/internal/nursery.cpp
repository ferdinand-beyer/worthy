#include "internal/nursery.h"


namespace worthy {
namespace internal {


Nursery::Nursery(Heap* heap, BlockAllocator* allocator)
    : Space(heap, allocator) {
}


} } // namespace worthy::internal
