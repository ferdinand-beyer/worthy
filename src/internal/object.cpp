#include "internal/object.h"

#include "internal/hash.h"
#include "internal/hashmap.h"
#include "internal/object-dispatch.h"
#include "internal/reference.h"
#include "internal/space.h"


namespace worthy {
namespace internal {


Object::Object(ObjectType type)
    : type_{type},
      flags_{0},
      page_marker_{0},
      control_{0} {
}


Heap* Object::heap() const {
    return Space::spaceOf(this)->heap();
}


HashCode Object::hashCode() const {
    DISPATCH_CONST(WORTHY_OBJECT_TYPES, _hashCode, ());
}


HashCode Object::_hashCode() const {
    return hash(reinterpret_cast<std::size_t>(this));
}


bool Object::equals(const Object* other) const {
    DISPATCH_CONST(WORTHY_OBJECT_TYPES, _equals, (other));
}


bool Object::_equals(const Object* other) const {
    return this == other;
}


} } // namespace worthy::internal
