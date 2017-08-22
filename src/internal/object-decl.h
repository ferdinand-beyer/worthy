#ifndef WORTHY_INTERNAL_OBJECT_DECL_H_
#define WORTHY_INTERNAL_OBJECT_DECL_H_


#include "internal/check.h"
#include "internal/object.h"


#define DECL_CAST(object_type)                                  \
    static inline object_type* cast(Object* obj) {              \
        WORTHY_CHECK(obj->is##object_type());                   \
        return static_cast<object_type*>(obj);                  \
    }                                                           \
    static inline const object_type* cast(const Object* obj) {  \
        WORTHY_CHECK(obj->is##object_type());                   \
        return static_cast<const object_type*>(obj);            \
    }


#endif // WORTHY_INTERNAL_OBJECT_DECL_H_
