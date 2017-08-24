#include "internal/heap.h"

#include "internal/check.h"
#include "internal/hashmap.h"
#include "internal/object-space.h"
#include "internal/object.h"


namespace worthy {
namespace internal {


Heap::Heap()
    : object_space_{std::make_unique<ObjectSpace>(this)} {

    empty_hash_map_ = make<HashMap>();
    empty_hash_map_bitmap_node_ = make<HashMapBitmapNode>();
}


Heap::~Heap() {
}


HashMap* Heap::emptyHashMap() const {
    return empty_hash_map_.get();
}


HashMapBitmapNode* Heap::emptyHashMapBitmapNode() const {
    return empty_hash_map_bitmap_node_.get();
}


} } // namespace worthy::internal

