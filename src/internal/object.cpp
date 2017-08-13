#include "internal/object.h"


namespace worthy {
namespace internal {


Object::Object(ObjectType type) : type_{type}, reserved_{0} {
}


} } // namespace worthy::internal
