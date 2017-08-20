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

    empty_hash_map_ = newReference(newObject<HashMap>());
    empty_hash_map_bitmap_node_ = newReference(newObject<HashMapBitmapNode>());
}


Heap::~Heap() {
}


Reference* Heap::newReference(Object* obj) {
    WORTHY_CHECK(obj);
    return reference_space_->newReference(obj);
}


HashMap* Heap::emptyHashMap() const {
    return HashMap::cast(empty_hash_map_->get());
}


HashMapBitmapNode* Heap::emptyHashMapBitmapNode() const {
    return HashMapBitmapNode::cast(empty_hash_map_bitmap_node_->get());
}


Reference* Heap::emptyHashMapReference() const {
    empty_hash_map_->retain();
    return empty_hash_map_;
}


} } // namespace worthy::internal

