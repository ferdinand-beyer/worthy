#ifndef WORTHY_INTERNAL_ROOT_SPACE_H_
#define WORTHY_INTERNAL_ROOT_SPACE_H_

#include "internal/macros.h"

#include <cstddef>

namespace worthy {
namespace internal {

class Page;
class Root;

class RootSpace {
public:
    static const std::size_t PageSizeBits = 16;
    static const std::size_t PageSize = 1 << PageSizeBits;
    static const std::size_t PageAlignment = 32;

    RootSpace();
    ~RootSpace();

    Root* newRoot(void* ptr);

private:
    NO_COPY(RootSpace);

    Page* allocatePage();

    Page* first_page_;
    Root* free_list_head_;
};

} } // namespace worty::internal

#endif // WORTHY_INTERNAL_ROOT_SPACE_H_
