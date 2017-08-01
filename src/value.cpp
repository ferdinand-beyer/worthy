#include "worthy/value.h"

namespace worthy {

Value::Value() : type_{TYPE_NULL} {
    data_.ptr = nullptr;
}

Value::Value(Type t) : type_{t} {
}

Value::Value(const Value& other) : data_{other.data_}, type_{other.type_} {
    incRef();
}

Value::Value(Value&& other) : type_{TYPE_NULL} {
    data_.ptr = nullptr;
    swap(other);
}

Value::~Value() {
    decRef();
}

Value& Value::operator=(const Value& other) {
    type_ = other.type_;
    data_ = other.data_;
    incRef();
    return *this;
}

Value& Value::operator=(Value&& other) {
    swap(other);
    return *this;
}

void Value::incRef() {
}

void Value::decRef() {
}

} // namespace worthy
