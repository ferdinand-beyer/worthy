#ifndef WORTHY_INTERNALS_H_
#define WORTHY_INTERNALS_H_


#include "internal/heap.h"
#include "internal/objects.h"


namespace worthy {


class Value;


Value toValue(const internal::Variant& v);


inline internal::Reference* newReference(internal::Object* obj) {
    return obj->heap()->newReference(obj);
}


bool equals(internal::Reference* lhs, internal::Reference* rhs);


} // namespace worthy


#endif // WORTHY_INTERNALS_H_
