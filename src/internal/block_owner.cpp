#include "internal/block_owner.h"

#include "internal/block.h"


namespace worthy {
namespace internal {


void BlockOwner::take(Block* block) {
    BlockOwnerAccess::take(this, block);
}


} } // namespace worthy::internal
