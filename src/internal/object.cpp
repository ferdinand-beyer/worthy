#include "internal/object.h"

#include "internal/hash.h"
#include "internal/hashmap.h"
#include "internal/object_dispatch.h"
#include "internal/object_header.h"


namespace worthy {
namespace internal {


ObjectType Object::type() const {
    return ObjectHeader::of(this)->type();
}


Heap* Object::heap() const {
    return ObjectHeader::of(this)->heap();
}


HashCode Object::hashCode() const {
    DISPATCH_CONST(WORTHY_OBJECT_TYPES, _hashCode, ());
}


HashCode Object::_hashCode() const {
    return hash(reinterpret_cast<uintptr_t>(this));
}


bool Object::equals(const Object* other) const {
    DISPATCH_CONST(WORTHY_OBJECT_TYPES, _equals, (other));
}


bool Object::_equals(const Object* other) const {
    return this == other;
}


} } // namespace worthy::internal
