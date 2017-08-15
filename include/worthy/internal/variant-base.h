#ifndef WORTHY_INTERNAL_VARIANT_BASE_H_
#define WORTHY_INTERNAL_VARIANT_BASE_H_


#include <cstdint>


namespace worthy {
namespace internal {


class Object;
class Reference;


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
    Type_Object,

    LastPrimitiveType = Type_Double
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
    Reference* ref;

    VariantData() : obj{nullptr} {}
    VariantData(bool init) : b{init} {}
    VariantData(std::int8_t init) : i8{init} {}
    VariantData(std::int16_t init) : i16{init} {}
    VariantData(std::int32_t init) : i32{init} {}
    VariantData(std::int64_t init) : i64{init} {}
    VariantData(std::uint8_t init) : u8{init} {}
    VariantData(std::uint16_t init) : u16{init} {}
    VariantData(std::uint32_t init) : u32{init} {}
    VariantData(std::uint64_t init) : u64{init} {}
    VariantData(float init) : f{init} {}
    VariantData(double init) : d{init} {}
    VariantData(Object* init) : obj{init} {}
    VariantData(Reference* init) : ref{init} {}
};


static_assert(sizeof(VariantData) == 8, "unexpected size");
static_assert(alignof(VariantData) == 8, "unexpected alignment");

} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_VARIANT_BASE_H_
