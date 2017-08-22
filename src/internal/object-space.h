#ifndef WORTHY_INTERNAL_OBJECT_SPACE_H_
#define WORTHY_INTERNAL_OBJECT_SPACE_H_


#include "internal/globals.h"
#include "internal/object-type.h"
#include "internal/space.h"


namespace worthy {
namespace internal {


class ObjectSpace final : public Space {
public:
    explicit ObjectSpace(Heap* heap);

    ~ObjectSpace();

    template<typename T>
    inline void* allocate() {
        return allocateInternal(ObjectTypeOf<T>(),
                                sizeof(T),
                                alignof(T));
    }

    template<typename T>
    inline void* allocateExtra(std::size_t extra_size) {
        return allocateInternal(ObjectTypeOf<T>(),
                                sizeof(T) + extra_size,
                                alignof(T));
    }

private:
    WORTHY_DISABLE_COPY(ObjectSpace);

    void* allocateInternal(ObjectType type,
                           std::size_t size,
                           std::size_t alignment);

    bool allocateRaw(std::size_t size,
                     std::size_t alignment,
                     Page*& page,
                     void*& memory);
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_OBJECT_SPACE_H_
