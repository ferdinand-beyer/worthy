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


template <typename Function>
typename Function::return_type dispatch(const VariantData& data, const VariantType& type, Function f) {
    switch (type) {
    case VariantType::Boolean:
        return f(data.b);
    case VariantType::Int8:
        return f(data.i8);
    case VariantType::Int16:
        return f(data.i16);
    case VariantType::Int32:
        return f(data.i32);
    case VariantType::Int64:
        return f(data.i64);
    case VariantType::UInt8:
        return f(data.u8);
    case VariantType::UInt16:
        return f(data.u16);
    case VariantType::UInt32:
        return f(data.u32);
    case VariantType::UInt64:
        return f(data.u64);
    case VariantType::Float:
        return f(data.f);
    case VariantType::Double:
        return f(data.d);
    case VariantType::Object:
        return f(data.obj);
    }
}


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_VARIANT_H_
