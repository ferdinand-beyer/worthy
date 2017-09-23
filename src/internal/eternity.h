#ifndef WORTHY_INTERNAL_ETERNITY_H_
#define WORTHY_INTERNAL_ETERNITY_H_


#include "internal/space.h"


namespace worthy {
namespace internal {


class HashMap;
class HashMapBitmapNode;


/*!
 * \brief Object space for immortal immutable objects.
 */
class Eternity final : public Space {
public:
    Eternity(Heap* heap, BlockAllocator* allocator);

    HashMap* emptyHashMap() const;
    HashMapBitmapNode* emptyHashMapBitmapNode() const;

private:
    HashMap* empty_hashmap_;
    HashMapBitmapNode* empty_hashmap_bitmap_node_;
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_ETERNITY_H_
