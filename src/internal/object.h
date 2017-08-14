#ifndef WORTHY_INTERNAL_OBJECT_H_
#define WORTHY_INTERNAL_OBJECT_H_


#include "internal/check.h"
#include "internal/variant.h"

#include <cstddef>
#include <cstdint>


#define WORTHY_ASSERT_SIZE(Cls) \
    static_assert(sizeof(Cls) == (Cls::Size + Cls::Padding), \
            "unexpected size of type " #Cls)


namespace worthy {
namespace internal {


// TODO: Move to different header
const std::size_t PointerSize = sizeof(void*);
const std::size_t WordSize = PointerSize;

typedef std::size_t HashCode;
typedef std::uint32_t ElementCount;

// TODO: Transients are TBD
typedef std::intptr_t TransientTag;


enum class ObjectType : std::uint8_t {
    FreeSpace,
    String,
    Binary,
    Symbol,
    List,
    Vector,
    VectorNode,
    HashMap,
    HashMapArrayNode,
    HashMapBitmapNode,
    HashMapCollisionNode,
    TransientHashMap
};


#define WORTHY_OBJECT_EACH_TYPE(M)  \
    M(FreeSpace)                    \
    M(String)                       \
    M(Binary)                       \
    M(Symbol)                       \
    M(List)                         \
    M(Vector)                       \
    M(VectorNode)                   \
    M(HashMap)                      \
    M(HashMapArrayNode)             \
    M(HashMapBitmapNode)            \
    M(HashMapCollisionNode)         \
    M(TransientHashMap)


#define WORTHY_OBJECT_MAKE_CAST(type) \
    static inline type* cast(Object* obj) {                 \
        WORTHY_CHECK(obj->is##type());                      \
        return static_cast<type*>(obj);                     \
    }                                                       \
    static inline const type* cast(const Object* obj) {     \
        WORTHY_CHECK(obj->is##type());                      \
        return static_cast<const type*>(obj);               \
    }


class Object {
public:
    static const std::size_t Size = 2;
    static const std::size_t Padding = 0;

    inline ObjectType type() const {
        return type_;
    }

#define IS_TYPE_FN(Type)                        \
    inline bool is##Type() const {              \
        return (type_ == ObjectType::Type);     \
    }
    WORTHY_OBJECT_EACH_TYPE(IS_TYPE_FN)
#undef IS_TYPE_FN

protected:
    Object(ObjectType type);

private:
    const ObjectType type_;
    std::uint8_t reserved_;
};


WORTHY_ASSERT_SIZE(Object);


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_OBJECT_H_
