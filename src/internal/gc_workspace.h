#ifndef WORTHY_INTERNAL_GC_WORKSPACE_H_
#define WORTHY_INTERNAL_GC_WORKSPACE_H_


#include "internal/block.h"
#include "internal/globals.h"


namespace worthy {
namespace internal {


class BlockAllocator;
class Generation;


class GCWorkspace {
public:
    GCWorkspace(Generation* generation, BlockAllocator* allocator);

    /// Allocate memory to copy an object into this generation.
    void* allocate(size_t size);

    /// Move all completed blocks to the Generation.
    void collectCompletedBlocks();

private:
    Generation* const generation_;
    BlockAllocator* const allocator_;

    /// Block where we copy objects to.
    Block* allocation_block_;

    /// List of completed blocks.
    BlockList done_blocks_;

    /// Number of copied objects.
    size_t object_count_;
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_GC_WORKSPACE_H_
