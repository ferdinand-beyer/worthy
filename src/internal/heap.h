#ifndef WORTHY_INTERNAL_HEAP_H_
#define WORTHY_INTERNAL_HEAP_H_

#include "internal/macros.h"

#include <memory>

namespace worthy {
namespace internal {

class ReferenceSpace;

class Heap {
public:
    Heap();
    ~Heap();

private:
    WORTHY_DISABLE_COPY(Heap);

    std::unique_ptr<ReferenceSpace> reference_space_;
};

} } // namespace worthy::internal

#endif // WORTHY_INTERNAL_HEAP_H_
