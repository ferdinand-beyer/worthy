#include "internal/eternity.h"

#include "internal/hashmap.h"


namespace worthy {
namespace internal {


Eternity::Eternity(Heap* heap, BlockAllocator* allocator)
        : Space(heap, allocator) {
    empty_hashmap_ = construct<HashMap>();
    empty_hashmap_bitmap_node_ = construct<HashMapBitmapNode>();
}


void Eternity::initBlock(Block& block) const {
    block.flags() |= Block::ImmortalFlag;
}


HashMap* Eternity::emptyHashMap() const {
    return empty_hashmap_;
}


HashMapBitmapNode* Eternity::emptyHashMapBitmapNode() const {
    return empty_hashmap_bitmap_node_;
}


} } // namespace worthy::internal
