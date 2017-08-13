#include "worthy/map.h"
#include "worthy/runtime.h"

#include "internal/runtime-impl.h"


namespace worthy {


Runtime::Runtime()
    : rt_{std::make_unique<internal::RuntimeImpl>()} {
}


Runtime::~Runtime() {
}


Map Runtime::map() {
    return rt_->emptyHashMapReference();
}


} // namespace worthy
