#include "worthy/map.h"
#include "worthy/runtime.h"

#include "internal/hashmap.h"
#include "internal/runtime-impl.h"


namespace worthy {


Runtime::Runtime()
    : rt_{std::make_unique<internal::RuntimeImpl>()} {
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
