#ifndef WORTHY_HEAP_PAGE_H_
#define WORTHY_HEAP_PAGE_H_

#include "internal/memory.h"

#include <atomic>

namespace worthy {
namespace internal {

class Space;

class Page {
public:
    Page(Space* owner, std::size_t size);

    Address allocate(std::size_t size);

private:
    std::size_t size_;
    Space* space_;
    Address free_list_;
};

} // namespace internal
} // namespace worthy

#endif // WORTHY_HEAP_PAGE_H_
