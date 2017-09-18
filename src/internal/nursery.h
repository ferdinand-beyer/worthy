#ifndef WORTHY_INTERNAL_NURSERY_H_
#define WORTHY_INTERNAL_NURSERY_H_


#include "internal/space.h"


namespace worthy {
namespace internal {


class BlockAllocator;


class Nursery final : public Space {
public:
    Nursery(Heap* heap, BlockAllocator* allocator);

    using Space::make;
    using Space::makeExtra;
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_NURSERY_H_
