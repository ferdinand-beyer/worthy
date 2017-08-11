#include "internal/memory.h"

#include <cstdlib>

namespace worthy {
namespace internal {

void* aligned_alloc(std::size_t size, std::size_t alignment) {
    void* result;
#ifdef _MSC_VER 
    result = _aligned_malloc(size, alignment);
#else 
    if (posix_memalign(&result, alignment, size)) {
        result = nullptr;
    }
#endif
    return result;
}

void aligned_free(void* ptr) {
#ifdef _MSC_VER 
    _aligned_free(ptr);
#else
    free(ptr);
#endif
}

} // namespace internal
} // namespace worthy
