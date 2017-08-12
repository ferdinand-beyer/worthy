#include "internal/page.h"

namespace worthy {
namespace internal {

Page::Page(Space* owner, std::size_t size) :
    size_{size},
    space_{owner},
    data_{reinterpret_cast<char*>(this) + sizeof(Page)}
{
}

void* Page::allocate(std::size_t size) {
    void* memory = data_;
    data_ = reinterpret_cast<char*>(data_) + size;
    return memory;
}

} } // namespace worthy::internal
