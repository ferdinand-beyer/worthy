#include "internal/heap.h"

#include "internal/reference-space.h"

namespace worthy {
namespace internal {

Heap::Heap() {
    reference_space_.reset(new ReferenceSpace(this));
}

Heap::~Heap() {
}

} } // namespace worthy::internal

