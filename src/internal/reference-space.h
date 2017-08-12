#ifndef WORTHY_INTERNAL_REFERENCE_SPACE_H_
#define WORTHY_INTERNAL_REFERENCE_SPACE_H_

#include "internal/macros.h"
#include "internal/space.h"

#include <atomic>
#include <cstddef>

namespace worthy {
namespace internal {

class Reference;

class ReferenceSpace : public Space {
public:
    static ReferenceSpace* ownerOf(Reference* ref);

    explicit ReferenceSpace(Heap* heap, std::size_t pageCapacity = 512);
    ~ReferenceSpace();

    Reference* newReference(void* ptr);

private:
    class Page;

    Reference* allocateFromFreeList(void* ptr);
    void addToFreeList(Reference* ref);

    Page* allocatePage();

    const std::size_t page_capacity_;
    Page* root_;
    std::atomic<Reference*> free_list_;

    WORTHY_DISABLE_COPY(ReferenceSpace);

    friend class Reference;
};

} } // namespace worty::internal

#endif // WORTHY_INTERNAL_REFERENCE_SPACE_H_
