#ifndef WORTHY_INTERNAL_SPACE_H_
#define WORTHY_INTERNAL_SPACE_H_


#include "internal/block.h"
#include "internal/block_owner.h"


namespace worthy {
namespace internal {


class BlockAllocator;
class Heap;
class Object;


class Space : public BlockOwner {
public:
    static Space* of(const Object* object);

    Space(const Space&) = delete;
    Space& operator=(const Space&) = delete;

    ~Space() override;

    Heap* heap() const;

protected:
    Space(Heap* heap, BlockAllocator* allocator);

    void* allocate(size_t size);

private:
    Block* blockForAllocation(size_t size);

    Heap* heap_;
    BlockAllocator* allocator_;
    BlockList blocks_;
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_SPACE_H_
