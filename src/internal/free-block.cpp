#include "internal/free-block.h"

namespace worthy {
namespace internal {

FreeBlock::FreeBlock(std::size_t size, FreeBlock* next)
        : size_{size}, next_{next} {
    // TODO
    //assert(size >= sizeof(*this))
}

Block::Structure FreeBlock::structure() const {
    Structure s { size_, alignof(FreeBlock) };
    return s;
}


} // namespace internal
} // namespace worthy
