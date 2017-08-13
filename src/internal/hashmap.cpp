#include "internal/hashmap.h"


namespace worthy {
namespace internal {


HashMap::HashMap()
    : Object(ObjectType::HashMap),
      has_null_key_{false},
      null_key_value_type_{VariantType::Null},
      count_{0},
      root_{nullptr} {
    null_key_value_.obj = nullptr;
}


} } // namespace worthy::internal
