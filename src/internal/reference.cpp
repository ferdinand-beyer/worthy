#include "internal/reference.h"

#include "internal/reference-space.h"


namespace worthy {
namespace internal {


Reference::Reference(Object* ptr)
    : Object(ObjectType::Reference),
      ptr_{ptr} {
}


ReferenceSpace* Reference::space() {
    return static_cast<ReferenceSpace*>(Space::spaceOf(this));
}


void Reference::retain() {
    space()->retain(this);
}


void Reference::release() {
    space()->release(this);
}


} } // namespace worthy::internal
