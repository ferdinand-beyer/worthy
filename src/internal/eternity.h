#ifndef WORTHY_INTERNAL_ETERNITY_H_
#define WORTHY_INTERNAL_ETERNITY_H_


#include "internal/handle.h"
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

    HandlePtr emptyHashMapHandle() const;

    HashMap* emptyHashMap() const;
    HashMapBitmapNode* emptyHashMapBitmapNode() const;

private:
    HandlePtr empty_hashmap_;
    HandlePtr empty_hashmap_bitmap_node_;
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_ETERNITY_H_
