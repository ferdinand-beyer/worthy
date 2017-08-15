#ifndef WORTHY_VALUE_H
#define WORTHY_VALUE_H


#include "worthy/abstract-value.h"


namespace worthy {


class Value final : public AbstractValue {
public:
    // Construct a Null value.
    Value();

    Value(bool b);

    Value(std::int8_t n);
    Value(std::int16_t n);
    Value(std::int32_t n);
    Value(std::int64_t n);

    Value(std::uint8_t n);
    Value(std::uint16_t n);
    Value(std::uint32_t n);
    Value(std::uint64_t n);

    Value(float n);
    Value(double n);

    Value(const AbstractValue& v);
    Value(AbstractValue&& v);

    Value& operator=(const AbstractValue& v);
    Value& operator=(AbstractValue&& v);

    void swap(Value& other);

    using AbstractValue::type;

    bool isNull() const;

    using AbstractValue::toBoolean;

    using AbstractValue::toInt8;
    using AbstractValue::toInt16;
    using AbstractValue::toInt32;
    using AbstractValue::toInt64;

    using AbstractValue::toUInt8;
    using AbstractValue::toUInt16;
    using AbstractValue::toUInt32;
    using AbstractValue::toUInt64;

    using AbstractValue::toFloat;
    using AbstractValue::toDouble;
};


inline void swap(Value& lhs, Value& rhs) {
    lhs.swap(rhs);
}


inline Value::Value() {}
inline Value::Value(bool b) : AbstractValue{b} {}
inline Value::Value(std::int8_t n) : AbstractValue{n} {}
inline Value::Value(std::int16_t n) : AbstractValue{n} {}
inline Value::Value(std::int32_t n) : AbstractValue{n} {}
inline Value::Value(std::int64_t n) : AbstractValue{n} {}
inline Value::Value(std::uint8_t n) : AbstractValue{n} {}
inline Value::Value(std::uint16_t n) : AbstractValue{n} {}
inline Value::Value(std::uint32_t n) : AbstractValue{n} {}
inline Value::Value(std::uint64_t n) : AbstractValue{n} {}
inline Value::Value(float n) : AbstractValue{n} {}
inline Value::Value(double n) : AbstractValue{n} {}
inline Value::Value(const AbstractValue& v) : AbstractValue{v} {}
inline Value::Value(AbstractValue&& v) : AbstractValue{v} {}


inline Value& Value::operator=(const AbstractValue& v) {
    AbstractValue::operator=(v);
    return *this;
}


inline Value& Value::operator=(AbstractValue&& v) {
    AbstractValue::operator=(v);
    return *this;
}


inline void Value::swap(Value& other) {
    AbstractValue::swap(other);
}


inline bool Value::isNull() const {
    return type() == Type::Null;
}


} // namespace worthy


#endif // WORTHY_VALUE_H
