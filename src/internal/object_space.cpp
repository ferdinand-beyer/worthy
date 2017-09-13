#include "internal/object_space.h"


namespace worthy {
namespace internal {


ObjectSpace::ObjectSpace(Heap* heap)
    : Space(heap) {
}


ObjectSpace::~ObjectSpace() {
}


void* ObjectSpace::allocateInternal(ObjectType type,
                                    std::size_t size,
                                    std::size_t alignment) {
    Page* page;
    void* memory;

    if (!allocateRaw(size + HeaderSize, alignment, page, memory)) {
        return nullptr;
    }

    return placeObjectHeader(memory, size, page, type);
}


bool ObjectSpace::allocateRaw(std::size_t size,
                              std::size_t alignment,
                              Page*& page,
                              void*& memory) {
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
