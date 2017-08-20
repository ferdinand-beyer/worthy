#include "worthy/runtime.h"

#include "worthy/map.h"

#include "internal/heap.h"


namespace worthy {


Runtime::Runtime()
    : heap_{std::make_unique<internal::Heap>()} {
}


Runtime::~Runtime() {
}


Map Runtime::map() {
    return heap_->emptyHashMapReference();
}


} // namespace worthy
