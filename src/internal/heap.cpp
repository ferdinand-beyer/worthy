#include "internal/heap.h"

#include "internal/check.h"
#include "internal/hashmap.h"
#include "internal/object-space.h"
#include "internal/object.h"
#include "internal/reference-space.h"
#include "internal/reference.h"


namespace worthy {
namespace internal {


Heap::Heap()
    : reference_space_{std::make_unique<ReferenceSpace>(this)},
      object_space_{std::make_unique<ObjectSpace>(this)} {

    empty_hash_map_ = newReference(newObject<HashMap>());
    empty_hash_map_bitmap_node_ = newReference(newObject<HashMapBitmapNode>());
}


Heap::~Heap() {
}


Reference* Heap::newReference(Object* obj) {
    WORTHY_DCHECK(obj);
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

