#ifndef WORTHY_INTERNAL_HEAP_H_
#define WORTHY_INTERNAL_HEAP_H_


#include "internal/globals.h"
#include "internal/handle_container.h"
#include "internal/nursery.h"
#include "internal/root_block_allocator.h"

#include <memory>


namespace worthy {
namespace internal {


class HashMap;
class HashMapBitmapNode;
class Object;


class Heap final {
public:
    Heap(const Heap&) = delete;
    Heap& operator=(const Heap&) = delete;

    Heap();
    ~Heap();

    Nursery* nursery();

    HandlePtr makeHandle(Object* obj);

    HandlePtr emptyHashMapHandle() const;
    HandlePtr emptyHashMapBitmapNodeHandle() const;

    HashMap* emptyHashMap() const;
    HashMapBitmapNode* emptyHashMapBitmapNode() const;

private:
    RootBlockAllocator allocator_;

    // TODO: Per-thread
    Nursery nursery_;
    HandleContainer handles_;

    HandlePtr empty_hashmap_;
    HandlePtr empty_hashmap_bitmap_node_;
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_HEAP_H_
