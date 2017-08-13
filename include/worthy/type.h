#ifndef WORTHY_TYPE_H
#define WORTHY_TYPE_H


#include "worthy/internal/variant-base.h"


namespace worthy {


enum class Type {
    Null    = internal::Type_Null,
    Boolean = internal::Type_Boolean,
    Int8    = internal::Type_Int8,
    Int16   = internal::Type_Int16,
    Int32   = internal::Type_Int32,
    Int64   = internal::Type_Int64,
    UInt8   = internal::Type_UInt8,
    UInt16  = internal::Type_UInt16,
    UInt32  = internal::Type_UInt32,
    UInt64  = internal::Type_UInt64,
    Float   = internal::Type_Float,
    Double  = internal::Type_Double,

    Map
};


} // namespace worthy


#endif // WORTHY_TYPE_H
