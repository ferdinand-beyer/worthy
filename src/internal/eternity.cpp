#include "internal/eternity.h"

#include "internal/hashmap.h"
#include "internal/heap.h"


namespace worthy {
namespace internal {


Eternity::Eternity(Heap* heap, BlockAllocator* allocator)
        : Space(heap, allocator) {
    empty_hashmap_ = heap->makeHandle(construct<HashMap>());
    empty_hashmap_bitmap_node_ =
        heap->makeHandle(construct<HashMapBitmapNode>());
}


HandlePtr Eternity::emptyHashMapHandle() const {
    return empty_hashmap_;
}


HashMap* Eternity::emptyHashMap() const {
    return HashMap::cast(empty_hashmap_->get());
}


HashMapBitmapNode* Eternity::emptyHashMapBitmapNode() const {
    return HashMapBitmapNode::cast(empty_hashmap_bitmap_node_->get());
}


} } // namespace worthy::internal
