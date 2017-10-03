#include "internal/nursery.h"

#include "internal/block.h"


namespace worthy {
namespace internal {


Nursery::Nursery(Heap* heap, BlockAllocator* allocator) :
    Space(heap, allocator, 0, 0)
{
}


} } // namespace worthy::internal
