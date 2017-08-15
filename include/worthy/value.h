#ifndef WORTHY_VALUE_H
#define WORTHY_VALUE_H


#include "worthy/abstract-value.h"


namespace worthy {


class Value : public AbstractValue {
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

    Value(const Value&) = default;
    Value(Value&&) = default;

    Value& operator=(const Value& other) = default;
    Value& operator=(Value&& other) = default;

    void swap(Value& other);
};


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


inline void Value::swap(Value& other) {
    AbstractValue::swap(other);
}


inline void swap(Value& lhs, Value& rhs) {
    lhs.swap(rhs);
}


} // namespace worthy


#endif // WORTHY_VALUE_H
