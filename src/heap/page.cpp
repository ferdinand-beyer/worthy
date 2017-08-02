#include "src/heap/page.h"

namespace worthy {
namespace internal {

Page::Page(Space* owner, std::size_t size) : size_{size}, space_{owner} {
}

} // namespace internal
} // namespace worthy
