#include "internal/object.h"

#include "internal/hash.h"
#include "internal/hashmap.h"
#include "internal/object-dispatch.h"
#include "internal/object-header.h"


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
    return hash(reinterpret_cast<std::size_t>(this));
}


bool Object::equals(const Object* other) const {
    DISPATCH_CONST(WORTHY_OBJECT_TYPES, _equals, (other));
}


bool Object::_equals(const Object* other) const {
    return this == other;
}


void intrusive_ptr_add_ref(Object* obj) {
    ObjectHeader::of(obj)->retain();
}


void intrusive_ptr_release(Object* obj) {
    ObjectHeader::of(obj)->release();
}


} } // namespace worthy::internal
