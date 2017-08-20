#ifndef WORTHY_INTERNALS_H_
#define WORTHY_INTERNALS_H_


#include "internal/heap.h"
#include "internal/objects.h"


namespace worthy {


inline internal::Reference* newReference(internal::Object* obj) {
    return obj->heap()->newReference(obj);
}


} // namespace worthy


#endif // WORTHY_INTERNALS_H_
