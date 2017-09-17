#include "internal/nursery.h"

#include "internal/check.h"
#include "internal/object.h"

#include <boost/align/align_up.hpp>


using boost::alignment::align_up;


namespace worthy {
namespace internal {


Nursery::Nursery(Heap* heap, BlockAllocator* allocator)
    : Space(heap, allocator) {
}


void* Nursery::allocateObject(ObjectType type, size_t size) {
    if (size >= LargeObjectThreshold) {
        WORTHY_UNIMPLEMENTED(); // TODO
    }
    const size_t real_size = align_up(size, ObjectAlignment);
    void* memory = allocate(real_size);
    Object* as_obj = reinterpret_cast<Object*>(memory);
    // TODO: Make "size in words" more obvious!
    as_obj->size_ = real_size / WordSize;
    as_obj->type_ = type;
    as_obj->flags_ = 0;
    return memory;
}


} } // namespace worthy::internal
