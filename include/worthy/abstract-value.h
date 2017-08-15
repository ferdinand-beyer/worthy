#ifndef WORTHY_ABSTRACT_VALUE_H
#define WORTHY_ABSTRACT_VALUE_H


#include "worthy/internal/variant-base.h"
#include "worthy/type.h"


namespace worthy {


namespace internal {
class Object;
class Reference;
}


class AbstractValue {
public:
    bool isNull() const;

protected:
    AbstractValue();

    AbstractValue(bool b);

    AbstractValue(std::int8_t n);
    AbstractValue(std::int16_t n);
    AbstractValue(std::int32_t n);
    AbstractValue(std::int64_t n);

    AbstractValue(std::uint8_t n);
    AbstractValue(std::uint16_t n);
    AbstractValue(std::uint32_t n);
    AbstractValue(std::uint64_t n);

    AbstractValue(float n);
    AbstractValue(double n);

    AbstractValue(Type t, internal::Reference* ref);

    AbstractValue(const AbstractValue& other);
    AbstractValue(AbstractValue&& other);

    AbstractValue& operator=(const AbstractValue& other);
    AbstractValue& operator=(AbstractValue&& other);

    ~AbstractValue();

    void swap(AbstractValue& other);

    Type type() const;

    bool toBoolean() const;

    std::int8_t toInt8() const;
    std::int16_t toInt16() const;
    std::int32_t toInt32() const;
    std::int64_t toInt64() const;

    std::uint8_t toUInt8() const;
    std::uint16_t toUInt16() const;
    std::uint32_t toUInt32() const;
    std::uint64_t toUInt64() const;

    float toFloat() const;
    double toDouble() const;

    const internal::Object* object() const;

private:
    void reset();
    void retain();
    void release();

    internal::VariantData data_;
    Type type_;
};


inline AbstractValue::AbstractValue()
    : type_{Type::Null} {}


inline AbstractValue::AbstractValue(bool b)
    : data_{b}, type_{Type::Boolean} {}


inline AbstractValue::AbstractValue(std::int8_t n)
    : data_{n}, type_{Type::Int8} {}


inline AbstractValue::AbstractValue(std::int16_t n)
    : data_{n}, type_{Type::Int16} {}


inline AbstractValue::AbstractValue(std::int32_t n)
    : data_{n}, type_{Type::Int32} {}


inline AbstractValue::AbstractValue(std::int64_t n)
    : data_{n}, type_{Type::Int64} {}


inline AbstractValue::AbstractValue(std::uint8_t n)
    : data_{n}, type_{Type::UInt8} {}


inline AbstractValue::AbstractValue(std::uint16_t n)
    : data_{n}, type_{Type::UInt16} {}


inline AbstractValue::AbstractValue(std::uint32_t n)
    : data_{n}, type_{Type::UInt32} {}


inline AbstractValue::AbstractValue(std::uint64_t n)
    : data_{n}, type_{Type::UInt64} {}


inline AbstractValue::AbstractValue(float n)
    : data_{n}, type_{Type::Float} {}


inline AbstractValue::AbstractValue(double n)
    : data_{n}, type_{Type::Double} {}


inline Type AbstractValue::type() const {
    return type_;
}


inline bool AbstractValue::isNull() const {
    return type_ == Type::Null;
}


} // namespace worthy


#endif // WORTHY_ABSTRACT_VALUE_H
