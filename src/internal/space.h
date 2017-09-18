#ifndef WORTHY_INTERNAL_SPACE_H_
#define WORTHY_INTERNAL_SPACE_H_


#include "internal/block.h"
#include "internal/block_owner.h"
#include "internal/object_type.h"


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
    static constexpr size_t LargeObjectThreshold = BlockSize * 8 / 10;
    static constexpr size_t ObjectAlignment = 8;

    static_assert((ObjectAlignment % WordSize) == 0, "invalid alignment");

    Space(Heap* heap, BlockAllocator* allocator);

    template<typename T, typename... Args>
    inline T* make(Args&&... args) {
        void* memory = allocateObject(ObjectTypeOf<T>(), sizeof(T));
        return new (memory) T(std::forward<Args>(args)...);
    }

    template<typename T, typename... Args>
    inline T* makeExtra(std::size_t extra_size, Args&&... args) {
        //static_assert((sizeof(T) % ObjectAlignment) == 0, "invalid size");
        void* memory = allocateObject(ObjectTypeOf<T>(), sizeof(T) + extra_size);
        return new (memory) T(std::forward<Args>(args)...);
    }

private:
    void* allocateObject(ObjectType type, size_t size);
    void* allocate(size_t size);
    Block* blockForAllocation(size_t size);

    Heap* heap_;
    BlockAllocator* allocator_;
    BlockList blocks_;
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_SPACE_H_
