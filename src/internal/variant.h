#ifndef WORTHY_INTERNAL_VARIANT_H_
#define WORTHY_INTERNAL_VARIANT_H_


#include "worthy/internal/primitive.h"

#include "internal/check.h"
#include "internal/globals.h"

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

#define WORTHY_TEMP(name, id, type, field) \
    bool is##name() const;
    WORTHY_FOR_EACH_VARIANT_TYPE(WORTHY_TEMP)
#undef WORTHY_TEMP

    bool isNull() const;

    bool operator==(const Variant& other) const;
    bool operator!=(const Variant& other) const;

private:
    VariantData data_;
    VariantType type_;
};


HashCode hash(const Variant& variant);


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


#define WORTHY_TEMP(name, id, type, field)  \
inline bool Variant::is##name() const {     \
    return type_ == VariantType::name;      \
}
    WORTHY_FOR_EACH_VARIANT_TYPE(WORTHY_TEMP)
#undef WORTHY_TEMP


inline bool Variant::isNull() const {
    return isObject() && (data_.obj == nullptr);
}


inline bool Variant::operator!=(const Variant& other) const {
    return !operator==(other);
}


class VariantArray {
public:
    static std::size_t sizeFor(std::size_t count);

    VariantArray(Address start, std::size_t count);

    Variant get(std::size_t index) const;
    void set(std::size_t index, const Variant& value);

private:
    const std::size_t count_;
    VariantData* const data_array_;
    VariantType* const type_array_;
};


inline std::size_t VariantArray::sizeFor(std::size_t count) {
    return count * (sizeof(VariantData) + sizeof(VariantType));
}


inline VariantArray::VariantArray(Address start, std::size_t count)
    : count_{count},
      data_array_{reinterpret_cast<VariantData*>(start)},
      type_array_{reinterpret_cast<VariantType*>(data_array_ + count)} {}


inline Variant VariantArray::get(std::size_t index) const {
    WORTHY_DCHECK(index < count_);
    return {type_array_[index], data_array_[index]};
}


inline void VariantArray::set(std::size_t index, const Variant& value) {
    WORTHY_DCHECK(index < count_);
    data_array_[index] = value.data();
    type_array_[index] = value.type();
}


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_VARIANT_H_
