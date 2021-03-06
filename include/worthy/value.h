#ifndef WORTHY_VALUE_H
#define WORTHY_VALUE_H


#include "worthy/abstract_value.h"

#include <cstddef>


namespace worthy {


/*!
 * \brief An immutable value.
 */
class Value final : public AbstractValue {
public:
    // Construct a Null value.
    Value();
    Value(std::nullptr_t);

#define WORTHY_TEMP(name, id, type, field) \
    Value(type field##_);
    WORTHY_FOR_EACH_PRIMITIVE_TYPE(WORTHY_TEMP)
#undef WORTHY_TEMP

    Value(const AbstractValue& v);
    Value(AbstractValue&& v);

    Value& operator=(const AbstractValue& v);
    Value& operator=(AbstractValue&& v);

    void swap(Value& other);

    using AbstractValue::type;

    bool isNull() const;

    bool operator==(const Value& other) const;
    bool operator!=(const Value& other) const;

#define WORTHY_TEMP(name, id, type, field) \
    type to##name() const;
    WORTHY_FOR_EACH_PRIMITIVE_TYPE(WORTHY_TEMP)
#undef WORTHY_TEMP
};


inline void swap(Value& lhs, Value& rhs) {
    lhs.swap(rhs);
}


inline Value::Value() {}


inline Value::Value(std::nullptr_t) {}


#define WORTHY_TEMP(name, id, type, field) \
inline Value::Value(type field##_) : AbstractValue{field##_} {}
    WORTHY_FOR_EACH_PRIMITIVE_TYPE(WORTHY_TEMP)
#undef WORTHY_TEMP


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


inline bool Value::operator==(const Value& other) const {
    return equals(other);
}


inline bool Value::operator!=(const Value& other) const {
    return !equals(other);
}


#define WORTHY_TEMP(name, id, type, field)  \
inline type Value::to##name() const {       \
    return toPrimitive<type>();             \
}
    WORTHY_FOR_EACH_PRIMITIVE_TYPE(WORTHY_TEMP)
#undef WORTHY_TEMP


} // namespace worthy


#endif // WORTHY_VALUE_H
