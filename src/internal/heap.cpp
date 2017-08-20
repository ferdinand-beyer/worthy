#include "internal/heap.h"

#include "internal/check.h"
#include "internal/object-space.h"
#include "internal/objects.h"
#include "internal/reference-space.h"


namespace worthy {
namespace internal {


Heap::Heap() {
    reference_space_.reset(new ReferenceSpace(this));
    object_space_.reset(new ObjectSpace(this));

    empty_hash_map_ = reference_space_->newReference(
            object_space_->allocateObject<HashMap>());
}


Heap::~Heap() {
}


Reference* Heap::newReference(Object* obj) {
    WORTHY_CHECK(obj);
    return reference_space_->newReference(obj);
}


Reference* Heap::emptyHashMap() {
    empty_hash_map_->retain();
    return empty_hash_map_;
}


} } // namespace worthy::internal

