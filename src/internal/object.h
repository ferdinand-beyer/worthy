#ifndef WORTHY_INTERNAL_OBJECT_H_
#define WORTHY_INTERNAL_OBJECT_H_


#include "internal/check.h"
#include "internal/object_type.h"

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/seq/for_each.hpp>

#include <new>


namespace worthy {
namespace internal {


class Heap;


class Object {
public:
    static bool equals(const Object* a, const Object* b);

    template<typename T, typename... Args>
    static T* construct(void* ptr, size_t size, Args&&... args) {
        WORTHY_DCHECK(size >= sizeof(T));
        WORTHY_DCHECK(reinterpret_cast<uintptr_t>(ptr) % alignof(T) == 0);

        preInit(ptr, size, ObjectTypeOf<T>());
        return new (ptr) T(std::forward<Args>(args)...);
    }

    Object& operator=(const Object&) = delete;

    ObjectType type() const;

#define WORTHY_TEMP(r, _, object_type) \
    bool BOOST_PP_CAT(is, object_type)() const;
    BOOST_PP_SEQ_FOR_EACH(WORTHY_TEMP, _, WORTHY_OBJECT_TYPES)
#undef WORTHY_TEMP

    Heap* heap() const;

    HashCode hashCode() const;

    bool equals(const Object* other) const;

protected:
    Object();

private:
    static void preInit(void* ptr, size_t size, ObjectType type);

    HashCode hashCode_() const;
    bool equals_(const Object* other) const;

    uint32_t size_;
    ObjectType type_;
    uint8_t flags_;

    friend class GarbageCollector;
};


inline HashCode hash(const Object* obj) {
    return obj ? obj->hashCode() : 0;
}


inline bool Object::equals(const Object* a, const Object* b) {
    return a ? a->equals(b) : !b;
}


#define WORTHY_TEMP(r, _, object_type)                          \
inline bool BOOST_PP_CAT(Object::is, object_type)() const {     \
    return (type() == ObjectType::object_type);                 \
}
    BOOST_PP_SEQ_FOR_EACH(WORTHY_TEMP, _, WORTHY_OBJECT_TYPES)
#undef WORTHY_TEMP


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_OBJECT_H_
