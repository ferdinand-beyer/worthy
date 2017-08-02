#ifndef WORTHY_MEMORY_H_
#define WORTHY_MEMORY_H_

#include <cstddef>
#include <cstdint>

namespace worthy {
namespace internal {

typedef std::uint8_t Byte;
typedef Byte* Address;


void* aligned_alloc(std::size_t size, std::size_t alignment);

void aligned_free(void* ptr);

} // namespace internal
} // namespace worthy

#endif // WORTHY_MEMORY_H_
