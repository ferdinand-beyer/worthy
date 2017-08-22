#ifndef WORTHY_INTERNAL_REFERENCE_SPACE_H_
#define WORTHY_INTERNAL_REFERENCE_SPACE_H_


#include "internal/globals.h"
#include "internal/space.h"

#include <atomic>
#include <cstddef>


namespace worthy {
namespace internal {


class Object;
class Reference;


class ReferenceSpace final : public Space {
public:
    explicit ReferenceSpace(Heap* heap, std::uint32_t page_capacity = 512);
    ~ReferenceSpace();

    Reference* newReference(Object* obj);

private:
    Reference* allocateFromPage(Page* page, Object* obj);
    Reference* allocateFromFreeList(Object* obj);

    void reclaim(Object* obj) override;

    Page* addPage();

    const std::uint32_t page_capacity_;
    std::atomic<Reference*> free_list_;

    WORTHY_DISABLE_COPY(ReferenceSpace);
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_REFERENCE_SPACE_H_
