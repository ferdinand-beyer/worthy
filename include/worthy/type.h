#ifndef WORTHY_TYPE_H
#define WORTHY_TYPE_H


#include "worthy/internal/primitive.h"


namespace worthy {


enum class Type {
    Null = 0,

#define WORTHY_TEMP(name, id, type, field) \
    name = id,
    WORTHY_FOR_EACH_PRIMITIVE_TYPE(WORTHY_TEMP)
#undef WORTHY_TEMP

    Symbol,
    String,
    List,
    Vector,
    Map,

    FirstReferenceType = Symbol
};


} // namespace worthy


#endif // WORTHY_TYPE_H
