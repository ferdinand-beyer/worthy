#ifndef WORHTY_INTERNAL_GLOBLALS_H_
#define WORHTY_INTERNAL_GLOBLALS_H_


#include <cstddef>
#include <cstdint>


namespace worthy {
namespace internal {


#define WORTHY_DISABLE_ASSIGN(T)        \
    T& operator=(const T&) = delete


#define WORTHY_DISABLE_COPY(T)          \
    T(const T&) = delete;               \
    WORTHY_DISABLE_ASSIGN(T)


#define WORTHY_NOOP ((void) 0)


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

typedef uint16_t PageMarker;

// TODO: Transients are TBD
typedef intptr_t TransientTag;


constexpr size_t PointerSize = sizeof(void*);
constexpr size_t WordSize = PointerSize;


template<typename T>
inline constexpr uintptr_t addressOf(const T* ptr) {
    return reinterpret_cast<uintptr_t>(ptr);
}


} } // namespace worthy::internal


#endif // WORHTY_INTERNAL_GLOBLALS_H_
