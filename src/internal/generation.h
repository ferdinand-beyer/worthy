#ifndef WORTHY_INTERNAL_GENERATION_H_
#define WORTHY_INTERNAL_GENERATION_H_


#include "internal/space.h"


namespace worthy {
namespace internal {


class Generation final : public Space {
public:
    Generation(size_t index, Heap* heap, BlockAllocator* allocator);

    size_t index() const;

private:
    const size_t index_;

    BlockList old_blocks_;
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_GENERATION_H_
