#ifndef WORTHY_INTERNAL_OBJECT_TYPE_H_
#define WORTHY_INTERNAL_OBJECT_TYPE_H_


#include <boost/preprocessor/seq/enum.hpp>

#include <cstdint>


namespace worthy {
namespace internal {


#define WORTHY_OBJECT_TYPES     \
    (FreeSpace)                 \
    (Reference)                 \
    (String)                    \
    (Binary)                    \
    (Symbol)                    \
    (List)                      \
    (Vector)                    \
    (VectorNode)                \
    (HashMap)                   \
    (HashMapArrayNode)          \
    (HashMapBitmapNode)         \
    (HashMapCollisionNode)      \
    (TransientHashMap)


enum class ObjectType : std::uint8_t {
    BOOST_PP_SEQ_ENUM(WORTHY_OBJECT_TYPES)
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_OBJECT_TYPE_H_
