#include "src/heap/heap.h"
#include "src/heap/root-space.h"

namespace worthy {
namespace internal {

Heap::Heap() {
    root_space_.reset(new RootSpace(this));
}

Heap::~Heap() {
}

} // namespace internal
} // namespace worthy

