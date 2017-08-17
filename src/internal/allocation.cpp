#include "internal/allocation.h"

#include "internal/check.h"
#include "internal/memory.h"

#include <cstdlib>


namespace worthy {
namespace internal {


void* allocateAligned(std::size_t size, std::size_t alignment) {
    WORTHY_CHECK(size > 0);
    WORTHY_CHECK(isPowerOfTwo(alignment));
    WORTHY_CHECK(alignment >= sizeof(void*));

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


void deallocateAligned(void* ptr) {
    WORTHY_CHECK(ptr);

#ifdef _MSC_VER 
    _aligned_free(ptr);
#else
    free(ptr);
#endif
}


} } // namespace worthy::internal
