#include "internal/object.h"

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


} } // namespace worthy::internal
