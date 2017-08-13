#include "internal/check.h"
#include "internal/heap.h"

#include "internal/reference-space.h"

#include <cstdlib>


namespace worthy {
namespace internal {


Heap::Heap() {
    reference_space_.reset(new ReferenceSpace(this));
}


Heap::~Heap() {
    for (void* memory : allocated_) {
        std::free(memory);
    }
    allocated_.clear();
}


void* Heap::allocate(std::size_t size) {
    std::lock_guard<std::mutex> lock(mutex_);
    // This is a very naive scheme used for testing only!
    void* memory = std::malloc(size);
    if (!memory) {
        return nullptr;
    }
    auto result = allocated_.insert(memory);
    WORTHY_CHECK(result.second);
    return memory;
}


} } // namespace worthy::internal

