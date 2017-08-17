#ifndef WORTHY_INTERNAL_OBJECT_H_
#define WORTHY_INTERNAL_OBJECT_H_


#include "internal/check.h"
#include "internal/object-type.h"
#include "internal/variant.h"

#include <cstddef>
#include <cstdint>

// Has to be the last include (doesn't have include guards)
#include "internal/object-macros.h"


namespace worthy {
namespace internal {


// TODO: Move to different header
const std::size_t PointerSize = sizeof(void*);
const std::size_t WordSize = PointerSize;

typedef std::size_t HashCode;
typedef std::uint32_t ElementCount;

// TODO: Transients are TBD
typedef std::intptr_t TransientTag;


class Object {
public:
    inline ObjectType type() const {
        return type_;
    }

    DECL_IS_TYPE(WORTHY_OBJECT_TYPES)

protected:
    Object(ObjectType type);

private:
    const ObjectType type_;
    std::uint8_t flags_;
    std::uint16_t page_offset_;
    std::uint32_t control_;

    friend class Space;
};


} } // namespace worthy::internal


#define UNDEF_OBJECT_MACROS
#include "internal/object-macros.h"


#endif // WORTHY_INTERNAL_OBJECT_H_
