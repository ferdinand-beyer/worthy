#include "src/heap/space.h"

namespace worthy {
namespace internal {

Space::Space(Heap* heap) : heap_{heap}, first_page_{nullptr} {
}

Space::~Space() {
}

} // namespace internal
} // namespace worthy
