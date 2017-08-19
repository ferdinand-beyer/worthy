#ifndef WORHTY_INTERNAL_GLOBLALS_H_
#define WORHTY_INTERNAL_GLOBLALS_H_


#include <cstddef>
#include <cstdint>


namespace worthy {
namespace internal {


typedef std::uint8_t Byte;
typedef Byte* Address;

typedef std::uint32_t HashCode;

typedef std::uint16_t PageMarker;

typedef std::uint32_t ElementCount;

// TODO: Transients are TBD
typedef std::intptr_t TransientTag;


const std::size_t PointerSize = sizeof(void*);
const std::size_t WordSize = PointerSize;


} } // namespace worthy::internal


#endif // WORHTY_INTERNAL_GLOBLALS_H_
