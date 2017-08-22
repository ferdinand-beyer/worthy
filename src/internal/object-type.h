#ifndef WORTHY_INTERNAL_OBJECT_TYPE_H_
#define WORTHY_INTERNAL_OBJECT_TYPE_H_


#include "internal/globals.h"

#include <boost/preprocessor/seq/enum.hpp>


namespace worthy {
namespace internal {


#define WORTHY_OBJECT_DERIVED   \
    (Reference)                 \
    (HashMap)                   \
    (TransientHashMap)


#define WORTHY_HASHMAPNODE_DERIVED  \
    (HashMapArrayNode)              \
    (HashMapBitmapNode)             \
    (HashMapCollisionNode)


#define WORTHY_OBJECT_TYPES     \
    WORTHY_OBJECT_DERIVED       \
    WORTHY_HASHMAPNODE_DERIVED


enum class ObjectType : std::uint8_t {
    BOOST_PP_SEQ_ENUM(WORTHY_OBJECT_TYPES)
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_OBJECT_TYPE_H_
