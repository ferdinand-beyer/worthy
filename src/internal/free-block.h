#ifndef WORTHY_HEAP_FREE_BLOCK_H_
#define WORTHY_HEAP_FREE_BLOCK_H_

#include "internal/block.h"

namespace worthy {
namespace internal {

class FreeBlock : public Block {
public:
    FreeBlock(std::size_t size, FreeBlock* next = nullptr);

    Structure structure() const override;

    FreeBlock* next();

private:
    std::size_t size_;
    FreeBlock* next_;
};

inline FreeBlock* FreeBlock::next() {
    return next_;
}

} // namespace internal
} // namespace worthy

#endif // WORTHY_HEAP_FREE_BLOCK_H_
