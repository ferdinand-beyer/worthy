#include "internal/hash.h"

#include <MurmurHash3.h>

#include <cmath>
#include <limits>


namespace worthy {
namespace internal {


HashCode hash(const void* buffer, size_t size) {
    uint32_t result;
    ::MurmurHash3_x86_32(buffer, size, 0, &result);
    return result;
}


HashCode hash(int64_t x) {
    return hash(&x, sizeof(x));
}


HashCode hash(uint64_t x) {
    return hash(&x, sizeof(x));
}


HashCode hash(double x) {
    if (x == -0.0) {
        x = 0.0;
    } else if (std::isnan(x)) {
        x = std::numeric_limits<double>::quiet_NaN();
    }
    return hash(&x, sizeof(x));
}


HashCode hash(intptr_t x) {
    return hash(&x, sizeof(x));
}


HashCode hash(uintptr_t x) {
    return hash(&x, sizeof(x));
}


} } // namespace worthy::internal
