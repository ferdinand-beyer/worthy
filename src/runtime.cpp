#include "worthy/runtime.h"

#include "worthy/map.h"

#include "internal/eternity.h"
#include "internal/hashmap.h"
#include "internal/heap.h"


using worthy::internal::Handle;
using worthy::internal::Heap;
using worthy::internal::Object;


namespace worthy {


namespace  {


inline Handle* makeHandle(Heap& heap, Object* object) {
    return heap.makeHandle(object).detach();
}


} // namespace


Runtime::Runtime()
    : heap_{std::make_unique<Heap>()} {
}


Runtime::~Runtime() {
}


Map Runtime::map() {
    return makeHandle(*heap_, heap_->eternity().emptyHashMap());
}


} // namespace worthy
