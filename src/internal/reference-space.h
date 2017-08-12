#ifndef WORTHY_INTERNAL_REFERENCE_SPACE_H_
#define WORTHY_INTERNAL_REFERENCE_SPACE_H_

#include "internal/macros.h"
#include "internal/space.h"

#include <cstddef>

namespace worthy {
namespace internal {

class Reference;

class ReferenceSpace : public Space {
public:
    explicit ReferenceSpace(Heap* heap);
    ~ReferenceSpace();

    bool owns(const Reference* ref) const;

    Reference* newReference(void* ptr);

private:
    struct Page;

    static const std::size_t PageCapacity = 512;

    Page* allocatePage(std::size_t capacity = PageCapacity);

    Page* root_;

    WORTHY_DISABLE_COPY(ReferenceSpace);
};

} } // namespace worty::internal

#endif // WORTHY_INTERNAL_REFERENCE_SPACE_H_
