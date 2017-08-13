#ifndef WORTHY_INTERNAL_VARIANT_BASE_H_
#define WORTHY_INTERNAL_VARIANT_BASE_H_


#include <cstdint>


namespace worthy {
namespace internal {


class Object;


enum VariantTypeEnum {
    Type_Null,
    Type_Boolean,
    Type_Int8,
    Type_Int16,
    Type_Int32,
    Type_Int64,
    Type_UInt8,
    Type_UInt16,
    Type_UInt32,
    Type_UInt64,
    Type_Float,
    Type_Double,
    Type_Object
};


union VariantData {
    bool b;
    std::int8_t i8;
    std::int16_t i16;
    std::int32_t i32;
    std::int64_t i64;
    std::uint8_t u8;
    std::uint16_t u16;
    std::uint32_t u32;
    std::uint64_t u64;
    float f;
    double d;
    Object* obj;
};


static_assert(sizeof(VariantData) == 8, "unexpected size");
static_assert(alignof(VariantData) == 8, "unexpected alignment");

} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_VARIANT_BASE_H_
