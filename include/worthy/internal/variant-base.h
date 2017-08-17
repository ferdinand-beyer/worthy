#ifndef WORTHY_INTERNAL_VARIANT_BASE_H_
#define WORTHY_INTERNAL_VARIANT_BASE_H_


#include <cstdint>


namespace worthy {
namespace internal {


class Object;
class Reference;


#define WORTHY_FOR_EACH_PRIMITIVE_TYPE(F)   \
    F(Bool, 1, bool, b)                     \
    F(Int8, 2, std::int8_t, i8)             \
    F(UInt8, 3, std::uint8_t, u8)           \
    F(Int16, 4, std::int16_t, i16)          \
    F(UInt16, 5, std::uint16_t, u16)        \
    F(Int32, 6, std::int32_t, i32)          \
    F(UInt32, 7, std::uint32_t, u32)        \
    F(Int64, 8, std::int64_t, i64)          \
    F(UInt64, 9, std::uint64_t, u64)        \
    F(Float32, 10, float, f32)              \
    F(Float64, 11, double, f64)


#define WORTHY_FOR_EACH_REFERENCE_TYPE(F) \
    F(ObjectPtr, 12, worthy::internal::Object*, obj)


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


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_VARIANT_BASE_H_
