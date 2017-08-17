#ifndef WORTHY_INTERNAL_MEMORY_H_
#define WORTHY_INTERNAL_MEMORY_H_


#include "internal/check.h"

#include <cstdint>


namespace worthy {
namespace internal {


template <typename Integer>
constexpr inline bool isPowerOfTwo(Integer n> {
    return (n != 0 && ((n & (n - 1)) == 0));
}


#define WORTHY_STATIC_ASSERT_POWER_OF_TWO(b) \
    static_assert(isPowerOfTwo(b), "boundary " #b " must be a power of two");


template <typename Ptr>
constexpr inline std::intptr_t offsetFrom(Ptr p) {
    return p - static_cast<Ptr>(0);
}


template <typename Ptr>
constexpr inline Ptr addressFrom(std::intptr_t offset) {
    return static_cast<Ptr>(static_cast<Ptr>(0) + offset);
}


template <typename Ptr>
inline Ptr roundDown(Ptr p, std::intptr_t b) {
    WORTHY_DCHECK(isPowerOfTwo(b));
    return addressFrom<Ptr>(offsetFrom(p) & -b);
}


template <std::intptr_t b, typename Ptr>
constexpr inline Ptr roundDown(Ptr p) {
    WORTHY_STATIC_ASSERT_POWER_OF_TWO(b);
    return addressFrom<Ptr>(offsetFrom(p) & -b);
}


template <typename Ptr>
inline Ptr roundUp(Ptr p, std::intptr_t b) {
    return roundDown<Ptr>(static_cast<Ptr>(p + b - 1), b);
}


template <std::intptr_t b, typename Ptr>
constexpr inline Ptr roundUp(Ptr p) {
    return roundDown<b, Ptr>(static_cast<Ptr>(p + b - 1));
}


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_MEMORY_H_
