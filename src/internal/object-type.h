#ifndef WORTHY_INTERNAL_OBJECT_TYPE_H_
#define WORTHY_INTERNAL_OBJECT_TYPE_H_


#include "internal/globals.h"

#include <boost/preprocessor/seq/enum.hpp>
#include <boost/preprocessor/seq/for_each.hpp>

#include <type_traits>


namespace worthy {
namespace internal {


#define WORTHY_OBJECT_DERIVED_TYPES \
    (HashMap)                       \
    (TransientHashMap)


#define WORTHY_HASHMAPNODE_DERIVED_TYPES    \
    (HashMapArrayNode)                      \
    (HashMapBitmapNode)                     \
    (HashMapCollisionNode)


#define WORTHY_OBJECT_TYPES             \
    WORTHY_OBJECT_DERIVED_TYPES         \
    WORTHY_HASHMAPNODE_DERIVED_TYPES


enum class ObjectType : std::uint8_t {
    BOOST_PP_SEQ_ENUM(WORTHY_OBJECT_TYPES)
};


template<typename T>
struct ObjectTypeOf;


#define WORTHY_TEMP(r, _, object_type)                              \
class object_type;                                                  \
template<>                                                          \
struct ObjectTypeOf<object_type>                                    \
    : std::integral_constant<ObjectType, ObjectType::object_type>   \
{};
    BOOST_PP_SEQ_FOR_EACH(WORTHY_TEMP, _, WORTHY_OBJECT_TYPES)
#undef WORTHY_TEMP


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_OBJECT_TYPE_H_
