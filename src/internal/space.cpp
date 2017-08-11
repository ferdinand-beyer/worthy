#include "internal/space.h"

namespace worthy {
namespace internal {

Space::Space(Heap* heap) : heap_{heap}, first_page_{nullptr} {
}

Space::~Space() {
}

Address Space::allocateBlock(std::size_t size, std::size_t alignment) {
    return nullptr;
}

} // namespace internal
} // namespace worthy
