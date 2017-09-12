#ifndef WORTHY_INTERNAL_HASH_H_
#define WORTHY_INTERNAL_HASH_H_


#include "worthy/internal/primitive.h"

#include "internal/globals.h"


namespace worthy {
namespace internal {


HashCode hash(const void* buffer, size_t size);

HashCode hash(int64_t x);
HashCode hash(uint64_t x);
HashCode hash(double x);

HashCode hash(intptr_t x);
HashCode hash(uintptr_t x);


inline HashCode hash(int32_t x) {
    return hash(int64_t(x));
}


inline HashCode hash(int16_t x) {
    return hash(int64_t(x));
}


inline HashCode hash(int8_t x) {
    return hash(int64_t(x));
}


inline HashCode hash(uint32_t x) {
    return hash(uint64_t(x));
}


inline HashCode hash(uint16_t x) {
    return hash(uint64_t(x));
}


inline HashCode hash(uint8_t x) {
    return hash(uint64_t(x));
}


inline HashCode hash(float x) {
    return hash(double(x));
}


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_HASH_H_
