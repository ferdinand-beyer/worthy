#include "internal/object.h"

#include "internal/byte_array.h"
#include "internal/hash.h"
#include "internal/hashmap.h"
#include "internal/nursery.h"
#include "internal/object_dispatch.h"
#include "internal/space.h"


namespace worthy {
namespace internal {


void Object::preInit(void* ptr, size_t size, ObjectType type) {
    WORTHY_DCHECK((size % WordSize) == 0);

    Object* self = reinterpret_cast<Object*>(ptr);
    self->size_in_words_ = size / WordSize;
    self->type_ = type;
    self->flags_ = 0;

    std::atomic_init<Object*>(&self->moved_, nullptr);
}


Object::Object() {
    // Members are initialized by preInit().
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


void Object::traverse(ObjectVisitor& visitor) {
    DISPATCH(WORTHY_OBJECT_TYPES, traverse_, (visitor));
}


HashCode Object::hashCode_() const {
    return hash(reinterpret_cast<uintptr_t>(this));
}


bool Object::equals_(const Object* other) const {
    return this == other;
}


void Object::traverse_(ObjectVisitor& visitor) {
}


} } // namespace worthy::internal
