#ifndef WORTHY_INTERNAL_VARIANT_H_
#define WORTHY_INTERNAL_VARIANT_H_


#include <cstdint>


namespace worthy {
namespace internal {


class Object;


enum class VariantType : std::uint8_t {
    Null,
    Boolean,
    Int8,
    UInt8,
    Int16,
    UInt16,
    Int32,
    UInt32,
    Int64,
    UInt64,
    Float,
    Double,
    Object
};


union VariantValue {
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


static_assert(sizeof(VariantValue) == 8, "unexpected size");
static_assert(alignof(VariantValue) == 8, "unexpected alignment");


class Variant {
private:
    VariantValue value_;
    VariantType type_;
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_VARIANT_H_
