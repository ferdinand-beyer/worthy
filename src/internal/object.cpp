#include "internal/object.h"


namespace worthy {
namespace internal {


Object::Object(ObjectType type)
    : type_{type},
      flags_{0},
      page_offset_{0} {
}


} } // namespace worthy::internal
