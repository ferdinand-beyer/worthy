#ifndef WORTHY_INTERNAL_OBJECT_H_
#define WORTHY_INTERNAL_OBJECT_H_


#include "internal/check.h"
#include "internal/globals.h"
#include "internal/object_type.h"

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/seq/for_each.hpp>

#include <atomic>
#include <new>


namespace worthy {
namespace internal {


class Heap;
class ObjectVisitor;


class alignas(WordSize) Object {
public:
    static bool equals(const Object* a, const Object* b);

    template<typename T, typename... Args>
    static T* emplace(void* ptr, size_t size, Args&&... args) {
        WORTHY_DCHECK(size >= sizeof(T));
        WORTHY_DCHECK(reinterpret_cast<uintptr_t>(ptr) % alignof(T) == 0);

        preInit(ptr, size, ObjectTypeOf<T>());
        return ::new (ptr) T(std::forward<Args>(args)...);
    }

    static void* operator new(size_t) = delete;
    static void operator delete(void*) = delete;

    Object& operator=(const Object&) = delete;

    ObjectType type() const;

#define WORTHY_TEMP(r, _, object_type) \
    bool BOOST_PP_CAT(is, object_type)() const;
    BOOST_PP_SEQ_FOR_EACH(WORTHY_TEMP, _, WORTHY_OBJECT_TYPES)
#undef WORTHY_TEMP

    Heap* heap() const;

    HashCode hashCode() const;

    bool equals(const Object* other) const;

    void traverse(ObjectVisitor& visitor);

protected:
    Object();

private:
    static void preInit(void* ptr, size_t size, ObjectType type);

    HashCode hashCode_() const;
    bool equals_(const Object* other) const;

    void traverse_(ObjectVisitor& visitor);

    uint32_t size_in_words_;
    ObjectType type_;
    uint8_t flags_;
    std::atomic<Object*> moved_;

    friend class GCWorker;
};


static_assert((sizeof(Object) % WordSize) == 0, "invalid object size");


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
