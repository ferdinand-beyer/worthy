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

    explicit Space(Heap* heap);

    virtual ~Space();

protected:
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
    virtual void reclaim(Object* obj);

    Page* allocatePage(std::size_t data_size);
    void deletePages();

    WORTHY_DISABLE_COPY(Space);

    Heap* heap_;
    PageList pages_;
    std::mutex mutex_;
};


inline Page* Space::firstPage() {
    return pages_.empty() ? nullptr : &pages_.front();
}


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_SPACE_H_
