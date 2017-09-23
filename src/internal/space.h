#ifndef WORTHY_INTERNAL_SPACE_H_
#define WORTHY_INTERNAL_SPACE_H_


#include "internal/block.h"
#include "internal/block_owner.h"
#include "internal/object.h"


namespace worthy {
namespace internal {


class BlockAllocator;
class Generation;
class Heap;
class Object;


class Space : public BlockOwner {
public:
    static Space* of(const Object* object);

    Space(const Space&) = delete;
    Space& operator=(const Space&) = delete;

    ~Space() override;

    Heap* heap() const;

    size_t objectCount() const;

    void setNextGeneration(Generation* generation);

protected:
    static constexpr size_t LargeObjectThreshold = BlockSize * 8 / 10;
    static constexpr size_t ObjectAlignment = 8;

    static_assert((ObjectAlignment % WordSize) == 0, "invalid alignment");

    Space(Heap* heap, BlockAllocator* allocator, uint16_t generation_number,
            uint16_t block_flags = 0);

    template<typename T, typename... Args>
    inline T* construct(Args&&... args) {
        return constructInternal<T>(sizeof(T), std::forward<Args>(args)...);
    }

    template<typename T, typename... Args>
    inline T* constructEx(size_t extra_size, Args&&... args) {
        static_assert((sizeof(T) % ObjectAlignment) == 0, "invalid size");
        return constructInternal<T>(sizeof(T) + extra_size,
                std::forward<Args>(args)...);
    }

    void reset();

private:
    template<typename T, typename... Args>
    inline T* constructInternal(size_t size, Args&&... args) {
        void* ptr = allocate(size);
        return Object::emplace<T>(ptr, size, std::forward<Args>(args)...);
    }

    void* allocate(size_t& size);

    Block* blockForAllocation(size_t size);

    Heap* const heap_;
    BlockAllocator* const allocator_;

    const uint16_t block_flags_;
    const uint16_t generation_number_;

    Generation* next_generation_;

    BlockList blocks_;
    size_t object_count_;

    friend class GarbageCollector;
    friend class GCWorker;
    friend class ObjectSpaceAccess;
};


class ObjectSpaceAccess {
private:
    static inline void* allocate(Space& space, size_t& size) {
        return space.allocate(size);
    }

    friend class Object;
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_SPACE_H_
