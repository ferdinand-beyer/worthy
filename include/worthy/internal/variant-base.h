#ifndef WORTHY_INTERNAL_VARIANT_BASE_H_
#define WORTHY_INTERNAL_VARIANT_BASE_H_


#include <cstdint>


namespace worthy {
namespace internal {


class Object;


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


#define WORTHY_FOR_EACH_VARIANT_TYPE(F) \
    WORTHY_FOR_EACH_PRIMITIVE_TYPE(F)   \
    F(ObjectPtr, 12, worthy::internal::Object*, obj)


static const int LastPrimitiveType = 11;


union VariantData {
#define WORTHY_TEMP(name, id, type, field) \
    type field;
    WORTHY_FOR_EACH_VARIANT_TYPE(WORTHY_TEMP);
#undef WORTHY_TEMP

    VariantData() : obj{nullptr} {}

#define WORTHY_TEMP(name, id, type, field) \
    explicit VariantData(type field##_) : field{field##_} {}
    WORTHY_FOR_EACH_VARIANT_TYPE(WORTHY_TEMP);
#undef WORTHY_TEMP
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_VARIANT_BASE_H_
