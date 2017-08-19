#include "internal/hash.h"

#include <MurmurHash3.h>


namespace worthy {
namespace internal {


HashCode hash(const void* buffer, std::size_t size) {
    std::uint32_t result;
    ::MurmurHash3_x86_32(buffer, size, 0, &result);
    return result;
}


} } // namespace worthy::internal
