#include "internal/heap.h"

#include "internal/check.h"
#include "internal/hashmap.h"


namespace worthy {
namespace internal {


Heap::Heap() :
    allocator_{},
    nursery_{this, &allocator_},
    handle_pool_{&allocator_} {

    empty_hashmap_ = handle_pool_.makeHandle(nursery_.make<HashMap>());
    empty_hashmap_bitmap_node_ =
        handle_pool_.makeHandle(nursery_.make<HashMapBitmapNode>());
}


Heap::~Heap() {
}


Nursery* Heap::nursery() {
    return &nursery_;
}


HandlePtr Heap::makeHandle(Object* obj) {
    WORTHY_DCHECK(obj);
    return handle_pool_.makeHandle(obj);
}


HandlePtr Heap::emptyHashMapHandle() const {
    return empty_hashmap_;
}


HashMap* Heap::emptyHashMap() const {
    return HashMap::cast(empty_hashmap_->get());
}


HashMapBitmapNode* Heap::emptyHashMapBitmapNode() const {
    return HashMapBitmapNode::cast(empty_hashmap_bitmap_node_->get());
}


} } // namespace worthy::internal

