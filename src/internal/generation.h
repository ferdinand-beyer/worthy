#ifndef WORTHY_INTERNAL_GENERATION_H_
#define WORTHY_INTERNAL_GENERATION_H_


#include "internal/space.h"

#include <mutex>


namespace worthy {
namespace internal {


class Generation final : public Space {
public:
    Generation(uint16_t generation_no, Heap* heap, BlockAllocator* allocator);

private:
    void deallocateOldBlocks();

    std::mutex mutex_;
    BlockList old_blocks_;

    friend class GarbageCollector;
    friend class GCWorkspace;
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_GENERATION_H_
