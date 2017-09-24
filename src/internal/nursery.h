#ifndef WORTHY_INTERNAL_NURSERY_H_
#define WORTHY_INTERNAL_NURSERY_H_


#include "internal/space.h"


namespace worthy {
namespace internal {


class Nursery final : public Space {
public:
    Nursery(Heap* heap, BlockAllocator* allocator);

    using Space::construct;
    using Space::constructEx;

private:
    void initBlock(Block& block) const override;
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_NURSERY_H_
