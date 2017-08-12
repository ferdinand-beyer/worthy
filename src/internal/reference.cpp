#include "internal/reference.h"

#include "internal/reference-space.h"

namespace worthy {
namespace internal {

Reference::Reference(std::uint32_t index, void* ptr) :
    ptr_{ptr},
    count_{1},
    index_{index}
{
}

void Reference::use() {
    ++count_;
}

void Reference::release() {
    --count_;
}

} } // namespace worthy::internal
