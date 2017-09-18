#ifndef WORHTY_INTERNAL_GLOBLALS_H_
#define WORHTY_INTERNAL_GLOBLALS_H_


#include <cstddef>
#include <cstdint>


namespace worthy {
namespace internal {


using std::size_t;
using std::ptrdiff_t;

using std::intptr_t;
using std::uintptr_t;

using std::int8_t;
using std::int16_t;
using std::int32_t;
using std::int64_t;

using std::uint8_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;


typedef uint8_t byte;
typedef std::uint_fast32_t uint;

typedef uint32_t HashCode;


constexpr size_t PointerSize = sizeof(void*);
constexpr size_t WordSize = PointerSize;


} } // namespace worthy::internal


#endif // WORHTY_INTERNAL_GLOBLALS_H_
