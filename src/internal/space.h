#ifndef WORTHY_INTERNAL_SPACE_H_
#define WORTHY_INTERNAL_SPACE_H_


#include "internal/globals.h"
#include "internal/object-header.h"
#include "internal/page.h"

#include <boost/intrusive/list.hpp>

#include <mutex>


namespace worthy {
namespace internal {


class Heap;
class Object;


class Space {
public:
    virtual ~Space();

    Heap* heap();

protected:
    typedef boost::intrusive::list<Page> PageList;

    static constexpr std::size_t HeaderSize = sizeof(ObjectHeader);

    explicit Space(Heap* heap);

    void* placeObjectHeader(void* memory, std::size_t size,
                            Page* page, ObjectType type);

    void* placeReferenceHeader(void* memory, Page* page);

    Page* firstPage();
    Page* addPage(std::size_t data_size);

private:
    WORTHY_DISABLE_COPY(Space);

    virtual void reclaim(Object* obj);

    Page* allocatePage(std::size_t data_size);
    void deletePages();

    Heap* heap_;
    PageList pages_;
    std::mutex mutex_;

    friend class ObjectHeader;
};


inline Heap* Space::heap() {
    return heap_;
}


inline Page* Space::firstPage() {
    return pages_.empty() ? nullptr : &pages_.front();
}


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_SPACE_H_
