#ifndef WORTHY_INTERNAL_NURSERY_H_
#define WORTHY_INTERNAL_NURSERY_H_


#include "internal/block.h"
#include "internal/object_type.h"
#include "internal/space.h"


namespace worthy {
namespace internal {


class BlockAllocator;


class Nursery final : public Space {
public:
    Nursery(const Nursery&) = delete;
    Nursery& operator=(const Nursery&) = delete;

    Nursery(Heap* heap, BlockAllocator* allocator);

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
    static constexpr size_t LargeObjectThreshold = BlockSize * 8 / 10;
    static constexpr size_t ObjectAlignment = 8;

    static_assert((ObjectAlignment % WordSize) == 0, "invalid alignment");

    void* allocateObject(ObjectType type, size_t size);
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_NURSERY_H_
