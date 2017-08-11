#include "worthy/map.h"
#include "worthy/runtime.h"

#include "internal/hashmap.h"
#include "internal/internal-runtime.h"

namespace worthy {
namespace internal {

InternalRuntime::InternalRuntime() {
}

InternalRuntime::~InternalRuntime() {
}

} // namespace internal

Runtime::Runtime()
    : internal_{std::make_unique<internal::InternalRuntime>()} {
}

Runtime::~Runtime() {
}

Map Runtime::map() {
    // Need to allocate a new Root
    // or support a special value type for "static" data
    //return internal_->emptyHashMap();
    return Map();
}

} // namespace worthy
