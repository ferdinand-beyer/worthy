#include "internal/eternity.h"

#include "internal/hashmap.h"
#include "internal/heap.h"


namespace worthy {
namespace internal {


Eternity::Eternity(Heap* heap, BlockAllocator* allocator)
        : Space(heap, allocator, -1, Block::ImmortalFlag) {
    empty_hashmap_ = construct<HashMap>();
    empty_hashmap_bitmap_node_ = construct<HashMapBitmapNode>();
}


HashMap* Eternity::emptyHashMap() const {
    return empty_hashmap_;
}


HashMapBitmapNode* Eternity::emptyHashMapBitmapNode() const {
    return empty_hashmap_bitmap_node_;
}


} } // namespace worthy::internal
