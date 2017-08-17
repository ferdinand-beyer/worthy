#ifndef WORTHY_INTERNAL_MEMORY_H_
#define WORTHY_INTERNAL_MEMORY_H_


#include "internal/check.h"

#include <cstdint>


namespace worthy {
namespace internal {


template <typename T>
constexpr inline bool isPowerOfTwo(T n) {
    return (n != 0 && ((n & (n - 1)) == 0));
}


#define WORTHY_STATIC_ASSERT_POWER_OF_TWO(b) \
    static_assert(isPowerOfTwo(b), "boundary " #b " must be a power of two");


template <typename T>
constexpr inline std::intptr_t offsetFrom(T x) {
    return x - static_cast<T>(0);
}


template <typename T>
constexpr inline T addressFrom(std::intptr_t offset) {
    return static_cast<T>(static_cast<T>(0) + offset);
}


template <typename T>
inline T roundDown(T x, std::intptr_t b) {
    WORTHY_DCHECK(isPowerOfTwo(b));
    return addressFrom<T>(offsetFrom(x) & -b);
}


template <std::intptr_t b, typename T>
constexpr inline T roundDown(T x) {
    WORTHY_STATIC_ASSERT_POWER_OF_TWO(b);
    return addressFrom<T>(offsetFrom(x) & -b);
}


template <typename T>
inline T roundUp(T x, std::intptr_t b) {
    return roundDown<T>(static_cast<T>(x + b - 1), b);
}


template <std::intptr_t b, typename T>
constexpr inline T roundUp(T x) {
    return roundDown<b, T>(static_cast<T>(x + b - 1));
}


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_MEMORY_H_
