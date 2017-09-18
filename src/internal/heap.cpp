#include "internal/heap.h"

#include "internal/check.h"


namespace worthy {
namespace internal {


Heap::Heap() :
    allocator_{},
    handle_pool_{&allocator_},
    eternity_{this, &allocator_},
    nursery_{this, &allocator_} {
}


Heap::~Heap() {
}


const Eternity& Heap::eternity() const {
    return eternity_;
}


Nursery* Heap::nursery() {
    return &nursery_;
}


HandlePtr Heap::makeHandle(Object* obj) {
    WORTHY_DCHECK(obj);
    return handle_pool_.makeHandle(obj);
}


} } // namespace worthy::internal
