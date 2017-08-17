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
    explicit Space(Heap* heap);

    virtual ~Space();

protected:
    typedef boost::intrusive::list<Page> PageList;

    void initRefCount(Object* obj);

    void retainRef(Object* obj);
    void releaseRef(Object* obj);

private:
    virtual void reclaim(Object* obj);

    void deletePages();

    WORTHY_DISABLE_COPY(Space);

    Heap* heap_;
    PageList pages_;
    std::mutex mutex_;
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_SPACE_H_
