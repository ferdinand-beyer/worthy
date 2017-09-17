#include "internal/heap.h"

#include "internal/check.h"
#include "internal/hashmap.h"
#include "internal/object.h"
#include "internal/object_space.h"


namespace worthy {
namespace internal {


Heap::Heap() :
    allocator_{},
    handles_{&allocator_},
    object_space_{std::make_unique<ObjectSpace>(this)} {

    empty_hashmap_ = makeHandle(make<HashMap>());
    empty_hashmap_bitmap_node_ = makeHandle(make<HashMapBitmapNode>());
}


Heap::~Heap() {
}


HandlePtr Heap::makeHandle(Object* obj) {
    WORTHY_DCHECK(obj);
    return handles_.newHandle(obj);
}


HandlePtr Heap::emptyHashMapHandle() const {
    return empty_hashmap_;
}


HandlePtr Heap::emptyHashMapBitmapNodeHandle() const {
    return empty_hashmap_bitmap_node_;
}


HashMap* Heap::emptyHashMap() const {
    return HashMap::cast(empty_hashmap_->get());
}


HashMapBitmapNode* Heap::emptyHashMapBitmapNode() const {
    return HashMapBitmapNode::cast(empty_hashmap_bitmap_node_->get());
}


} } // namespace worthy::internal

