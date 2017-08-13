#ifndef WORTHY_INTERNAL_VARIANT_H_
#define WORTHY_INTERNAL_VARIANT_H_


#include "worthy/internal/variant-base.h"


namespace worthy {
namespace internal {


enum class VariantType : std::uint8_t {
    Null    = Type_Null,
    Boolean = Type_Boolean,
    Int8    = Type_Int8,
    Int16   = Type_Int16,
    Int32   = Type_Int32,
    Int64   = Type_Int64,
    UInt8   = Type_UInt8,
    UInt16  = Type_UInt16,
    UInt32  = Type_UInt32,
    UInt64  = Type_UInt64,
    Float   = Type_Float,
    Double  = Type_Double,
    Object  = Type_Object
};


class Variant {
private:
    VariantData data_;
    VariantType type_;
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_VARIANT_H_
