#include "internal/reference.h"

#include "internal/object-header.h"


namespace worthy {
namespace internal {


Reference::Reference(Object* ptr)
    : ptr_{ptr} {
}


void Reference::retain() {
    ObjectHeader::of(this)->retain();
}


void Reference::release() {
    ObjectHeader::of(this)->release();
}


} } // namespace worthy::internal
