#include "internal/hashmap.h"

namespace worthy {
namespace internal {

HashMap::HashMap() : count_{0}, root_{nullptr}, has_null_{false} {
    null_value_.ptr = nullptr;
}

HashMap::HashMap(std::size_t count, void* root, bool has_null, ValueData null_value)
    : count_{count},
      root_{root},
      has_null_{has_null},
      null_value_{null_value} {
}

} // namespace internal
} // namespace worthy
