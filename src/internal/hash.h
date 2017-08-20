#ifndef WORTHY_INTERNAL_HASH_H_
#define WORTHY_INTERNAL_HASH_H_


#include "worthy/internal/primitive.h"

#include "internal/globals.h"
#include "internal/objects.h"


namespace worthy {
namespace internal {


HashCode hash(const void* buffer, std::size_t size);


inline HashCode hash(std::size_t x) {
    return hash(&x, sizeof(x));
}


#define WORTHY_TEMP(name, id, type, field)  \
    inline HashCode hash(type x) {          \
        return hash(&x, sizeof(x));         \
    }
    WORTHY_FOR_EACH_PRIMITIVE_TYPE(WORTHY_TEMP)
#undef WORTHY_TEMP


inline HashCode hash(const Object* obj) {
    return obj ? obj->hashCode() : 0;
}


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_HASH_H_
