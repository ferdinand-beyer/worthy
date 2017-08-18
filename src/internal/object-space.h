#ifndef WORTHY_INTERNAL_OBJECT_SPACE_H_
#define WORTHY_INTERNAL_OBJECT_SPACE_H_


#include "internal/macros.h"
#include "internal/space.h"

#include <new>
#include <type_traits>


namespace worthy {
namespace internal {


class ObjectSpace final : public Space {
public:
    explicit ObjectSpace(Heap* heap);

    ~ObjectSpace();

    template<typename T, typename... Args>
    T* allocateObject(Args&&... args) {
        static_assert(std::is_base_of<Object, T>::value,
            "can only allocate Object-derived instances");

        void* memory;
        Page* page;

        if (!allocate(sizeof(T), alignof(T), memory, page)) {
            return nullptr;
        }

        T* object = new (memory) T(std::forward<Args>(args)...);

        initialize(object, page);

        return object;
    }

private:
    WORTHY_DISABLE_COPY(ObjectSpace);

    bool allocate(
            std::size_t size,
            std::size_t alignment,
            void*& memory,
            Page*& page);
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_OBJECT_SPACE_H_
