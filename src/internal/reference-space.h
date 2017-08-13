#ifndef WORTHY_INTERNAL_REFERENCE_SPACE_H_
#define WORTHY_INTERNAL_REFERENCE_SPACE_H_


#include "internal/macros.h"
#include "internal/space.h"

#include <atomic>
#include <cstddef>
#include <mutex>


namespace worthy {
namespace internal {


class Object;
class Reference;


class ReferenceSpace : public Space {
public:
    static ReferenceSpace* ownerOf(Reference* ref);

    explicit ReferenceSpace(Heap* heap, std::uint32_t page_capacity = 512);
    ~ReferenceSpace();

    // TODO -- will break unit tests
    //Reference* newReference(Object* obj);
    Reference* newReference(void* ptr);

private:
    class Page;

    Reference* allocateFromFreeList(void* ptr);

    void addToFreeList(Reference* ref);

    Page* allocatePageSync(Page* top_page);
    Page* allocatePage();

    const std::uint32_t page_capacity_;
    std::atomic<Page*> top_page_;
    std::atomic<Reference*> free_list_;
    std::mutex mutex_;

    WORTHY_DISABLE_COPY(ReferenceSpace);

    friend class Reference;
};


} } // namespace worty::internal


#endif // WORTHY_INTERNAL_REFERENCE_SPACE_H_
