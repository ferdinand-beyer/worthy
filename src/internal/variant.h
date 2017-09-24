#ifndef WORTHY_INTERNAL_VARIANT_H_
#define WORTHY_INTERNAL_VARIANT_H_


#include "worthy/internal/primitive.h"

#include "internal/check.h"
#include "internal/globals.h"

#include <boost/preprocessor/seq/enum.hpp>


namespace worthy {
namespace internal {


class GCVisitor;
class Object;


#define WORTHY_FOR_EACH_VARIANT_TYPE(F)             \
    F(Object, 0, worthy::internal::Object*, obj)    \
    WORTHY_FOR_EACH_PRIMITIVE_TYPE(F)


enum class VariantType : uint8_t {
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


/**
 * Tagged union for primitives and Objects.
 */
class Variant final {
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

    Object* toObject() const;

    void scan(GCVisitor& visitor);

private:
    VariantData data_;
    VariantType type_;
};


/**
 * Compute the hash code for a Variant.
 */
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


inline Object* Variant::toObject() const {
    WORTHY_DCHECK(isObject());
    return data_.obj;
}


/**
 * A compact Variant array.
 *
 * VariantData takes a whole word, whereas VariantType is only one byte
 * wide.  Therefore an array of Variant objects will waste 7 bytes per
 * object on a 64-bit system.
 *
 * The VariantArray class interprets a memory buffer as two consecutive
 * arrays: a VariantData array and a VariantType array, improving memory
 * usage.
 */
class VariantArray final {
public:
    static size_t sizeFor(size_t length);

    VariantArray(void* start, size_t length);

    size_t length() const;

    Variant get(size_t index) const;

    void set(size_t index, const Variant& value);

    void clear();

    void copy(const VariantArray& src);

    void copy(size_t dst_index,
              const VariantArray& src,
              size_t src_index,
              size_t length);

    void scan(GCVisitor& visitor);

private:
    const size_t length_;
    VariantData* const data_array_;
    VariantType* const type_array_;
};


inline size_t VariantArray::sizeFor(size_t length) {
    return length * (sizeof(VariantData) + sizeof(VariantType));
}


inline VariantArray::VariantArray(void* start, size_t length)
    : length_{length},
      data_array_{reinterpret_cast<VariantData*>(start)},
      type_array_{reinterpret_cast<VariantType*>(data_array_ + length)} {}


inline size_t VariantArray::length() const {
    return length_;
}


inline Variant VariantArray::get(size_t index) const {
    WORTHY_DCHECK(index < length_);
    return {type_array_[index], data_array_[index]};
}


inline void VariantArray::set(size_t index, const Variant& value) {
    WORTHY_DCHECK(index < length_);
    data_array_[index] = value.data();
    type_array_[index] = value.type();
}


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_VARIANT_H_
