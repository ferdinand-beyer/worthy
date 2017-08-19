#ifndef WORTHY_INTERNAL_OBJECT_H_
#define WORTHY_INTERNAL_OBJECT_H_


#include "internal/check.h"
#include "internal/globals.h"
#include "internal/object-type.h"
#include "internal/variant.h"

#include <cstddef>
#include <cstdint>

// Has to be the last include (doesn't have include guards)
#include "internal/object-macros.h"


namespace worthy {
namespace internal {


class Heap;


class Object {
public:
    ObjectType type() const;

    DECL_IS_TYPE(WORTHY_OBJECT_TYPES)

    Heap* heap() const;

    HashCode hashCode() const;

    bool equals(const Object* other) const;

protected:
    Object(ObjectType type);

private:
    const ObjectType type_;
    std::uint8_t flags_;
    PageMarker page_marker_;
    std::uint32_t control_;

    friend class Space;
};


inline ObjectType Object::type() const {
    return type_;
}


} } // namespace worthy::internal


#define UNDEF_OBJECT_MACROS
#include "internal/object-macros.h"


#endif // WORTHY_INTERNAL_OBJECT_H_
