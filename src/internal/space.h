#ifndef WORTHY_INTERNAL_SPACE_H_
#define WORTHY_INTERNAL_SPACE_H_


#include "internal/macros.h"
#include "internal/page.h"

#include <boost/intrusive/list.hpp>

#include <mutex>


namespace worthy {
namespace internal {


class Heap;
class Object;


class Space {
public:
    static Space* spaceOf(Object* obj);

    virtual ~Space();

    Heap* heap();

protected:
    explicit Space(Heap* heap);

    typedef boost::intrusive::list<Page> PageList;

    static Page* pageOf(Object* obj);

    void initialize(Object* obj, Page* page);

    void initRefCount(Object* obj);

    std::uint32_t refCount(Object* obj);

    void retainRef(Object* obj);
    void releaseRef(Object* obj);

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
};


inline Heap* Space::heap() {
    return heap_;
}


inline Page* Space::firstPage() {
    return pages_.empty() ? nullptr : &pages_.front();
}


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_SPACE_H_
