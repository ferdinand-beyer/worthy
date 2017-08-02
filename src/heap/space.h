#ifndef WORTHY_HEAP_SPACE_H_
#define WORTHY_HEAP_SPACE_H_

#include <cstddef>

namespace worthy {
namespace internal {

class Heap;
class Page;

class Space {
public:
    explicit Space(Heap* heap);

    Space(const Space&) = delete;

    virtual ~Space();

protected:
    Page* allocatePage(std::size_t size);

private:
    Heap* heap_;
    Page* first_page_;
};

} // namespace internal
} // namespace worthy

#endif // WORTHY_HEAP_SPACE_H_
