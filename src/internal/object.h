#ifndef WORTHY_INTERNAL_OBJECT_H_
#define WORTHY_INTERNAL_OBJECT_H_


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
    ByteArray,
    Symbol,
    List,
    Vector,
    VectorNode,
    HashMap,
    HashMapArrayNode,
    HashMapBitmapNode,
    HashMapCollisionNode,
    TransientHashMapNode
};


class Object {
public:
    static const std::size_t Size = 2;
    static const std::size_t Padding = 0;

protected:
    Object(ObjectType type);

private:
    const ObjectType type_;
    std::uint8_t reserved_;
};


WORTHY_ASSERT_SIZE(Object);


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_OBJECT_H_
