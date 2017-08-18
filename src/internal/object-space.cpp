#include "internal/object-space.h"


namespace worthy {
namespace internal {


ObjectSpace::ObjectSpace(Heap* heap)
    : Space(heap) {
}


ObjectSpace::~ObjectSpace() {
}


bool ObjectSpace::allocate(std::size_t size,
                           std::size_t alignment,
                           void*& memory,
                           Page*& page) {
    page = firstPage();
    if (page) {
        memory = page->allocate(size, alignment);
        if (memory) {
            return true;
        }
    }

    // TODO: What if size is huge?
    page = addPage(65536);
    if (!page) {
        return false;
    }

    memory = page->allocate(size, alignment);
    return memory != nullptr;
}


} } // namespace worthy::internal
