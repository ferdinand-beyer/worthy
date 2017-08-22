#include "internal/reference.h"

#include "internal/reference-space.h"


namespace worthy {
namespace internal {


namespace {


inline ReferenceSpace* spaceOf(Reference* ref) {
    return static_cast<ReferenceSpace*>(Space::spaceOf(ref));
}


} // namespace


Reference::Reference(Object* ptr)
    : Object(ObjectType::Reference),
      ptr_{ptr} {
}


void Reference::retain() {
    spaceOf(this)->retain(this);
}


void Reference::release() {
    spaceOf(this)->release(this);
}


} } // namespace worthy::internal
