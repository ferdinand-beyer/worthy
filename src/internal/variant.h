#ifndef WORTHY_INTERNAL_VARIANT_H_
#define WORTHY_INTERNAL_VARIANT_H_


#include "worthy/internal/primitive.h"

#include <boost/preprocessor/seq/enum.hpp>


namespace worthy {
namespace internal {


class Object;


#define WORTHY_FOR_EACH_VARIANT_TYPE(F)             \
    F(Object, 0, worthy::internal::Object*, obj)    \
    WORTHY_FOR_EACH_PRIMITIVE_TYPE(F)


enum class VariantType : std::uint8_t {
#define WORTHY_TEMP(name, id, type, field) (name = id)
    BOOST_PP_SEQ_ENUM(WORTHY_FOR_EACH_VARIANT_TYPE(WORTHY_TEMP))
#undef WORTHY_TEMP
};


union VariantData {
#define WORTHY_TEMP(name, id, type, field) \
    type field;
    WORTHY_FOR_EACH_VARIANT_TYPE(WORTHY_TEMP)
#undef WORTHY_TEMP

    VariantData() : obj{nullptr} {}

#define WORTHY_TEMP(name, id, type, field) \
    explicit VariantData(type field##_) : field{field##_} {}
    WORTHY_FOR_EACH_VARIANT_TYPE(WORTHY_TEMP)
#undef WORTHY_TEMP
};


template <typename Function>
inline typename Function::return_type
dispatch(const VariantData& data, const VariantType& type, Function f) {
    switch (type) {
#define WORTHY_TEMP(name, id, type, field) \
    case VariantType::name: \
        return f(data.field);
    WORTHY_FOR_EACH_VARIANT_TYPE(WORTHY_TEMP)
#undef WORTHY_TEMP
    }
}


class Variant {
public:
    Variant();

#define WORTHY_TEMP(name, id, type, field) \
    Variant(type x);
    WORTHY_FOR_EACH_VARIANT_TYPE(WORTHY_TEMP)
#undef WORTHY_TEMP

    Variant(VariantType type, VariantData data);

    VariantType type() const;
    VariantData data() const;

    bool isNull() const;

    bool operator==(const Variant& other) const;
    bool operator!=(const Variant& other) const;

private:
    VariantData data_;
    VariantType type_;
};


inline Variant::Variant()
    : data_{}, type_{VariantType::Object} {}


#define WORTHY_TEMP(name, id, type, field)  \
inline Variant::Variant(type x)             \
    : data_{x}, type_{VariantType::name} {}
    WORTHY_FOR_EACH_VARIANT_TYPE(WORTHY_TEMP)
#undef WORTHY_TEMP


inline Variant::Variant(VariantType type, VariantData data)
    : data_{data}, type_{type} {}


inline VariantType Variant::type() const {
    return type_;
}


inline VariantData Variant::data() const {
    return data_;
}


inline bool Variant::isNull() const {
    return (type_ == VariantType::Object) && (data_.obj == nullptr);
}


inline bool Variant::operator!=(const Variant& other) const {
    return !operator==(other);
}


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_VARIANT_H_
