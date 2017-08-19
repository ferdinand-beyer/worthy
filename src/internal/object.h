#ifndef WORTHY_INTERNAL_OBJECT_H_
#define WORTHY_INTERNAL_OBJECT_H_


#include "internal/check.h"
#include "internal/globals.h"
#include "internal/object-type.h"
#include "internal/variant.h"

#include <cstddef>
#include <cstdint>

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/seq/for_each.hpp>


namespace worthy {
namespace internal {


class Heap;


class Object {
public:
    static bool equals(const Object* a, const Object* b);

    ObjectType type() const;

#define WORTHY_TEMP(r, _, object_type) \
    bool BOOST_PP_CAT(is, object_type)() const;
    BOOST_PP_SEQ_FOR_EACH(WORTHY_TEMP, _, WORTHY_OBJECT_TYPES)
#undef WORTHY_TEMP

    Heap* heap() const;

    HashCode hashCode() const;

    bool equals(const Object* other) const;

protected:
    Object(ObjectType type);

private:
    HashCode _hashCode() const;
    bool _equals(const Object* other) const;

    const ObjectType type_;
    std::uint8_t flags_;
    PageMarker page_marker_;
    std::uint32_t control_;

    friend class Space;
};


inline bool Object::equals(const Object* a, const Object* b) {
    return a ? a->equals(b) : !b;
}


inline ObjectType Object::type() const {
    return type_;
}


#define WORTHY_TEMP(r, _, object_type)                          \
inline bool BOOST_PP_CAT(Object::is, object_type)() const {     \
    return (type() == ObjectType::object_type);                 \
}
    BOOST_PP_SEQ_FOR_EACH(WORTHY_TEMP, _, WORTHY_OBJECT_TYPES)
#undef WORTHY_TEMP


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_OBJECT_H_
