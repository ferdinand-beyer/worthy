#ifndef WORTHY_INTERNAL_GENERATION_H_
#define WORTHY_INTERNAL_GENERATION_H_


#include "internal/space.h"


namespace worthy {
namespace internal {


class Generation final : public Space {
public:
    Generation(uint16_t number, Heap* heap, BlockAllocator* allocator);

private:
    BlockList old_blocks_;

    friend class GarbageCollector;
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_GENERATION_H_
