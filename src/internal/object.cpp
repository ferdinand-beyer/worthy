#include "internal/object.h"

#include "internal/hash.h"
#include "internal/hashmap.h"
#include "internal/object_dispatch.h"
#include "internal/space.h"


namespace worthy {
namespace internal {


Object::Object() {
    // NOTE: members are initialized by the nursery!
}


ObjectType Object::type() const {
    return type_;
}


Heap* Object::heap() const {
    return Space::of(this)->heap();
}


HashCode Object::hashCode() const {
    DISPATCH_CONST(WORTHY_OBJECT_TYPES, hashCode_, ());
}


bool Object::equals(const Object* other) const {
    DISPATCH_CONST(WORTHY_OBJECT_TYPES, equals_, (other));
}


HashCode Object::hashCode_() const {
    return hash(reinterpret_cast<uintptr_t>(this));
}


bool Object::equals_(const Object* other) const {
    return this == other;
}


} } // namespace worthy::internal
