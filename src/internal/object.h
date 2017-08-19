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


typedef std::uint32_t ElementCount;

// TODO: Transients are TBD
typedef std::intptr_t TransientTag;


class Object {
public:
    ObjectType type() const;

    Heap* heap() const;

    DECL_IS_TYPE(WORTHY_OBJECT_TYPES)

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
