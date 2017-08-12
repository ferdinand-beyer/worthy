#include "internal/space.h"

namespace worthy {
namespace internal {

Space::Space(Heap* heap) : heap_{heap} {
}

Space::~Space() {
}

} } // namespace worthy::internal
