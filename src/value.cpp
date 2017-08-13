#include "worthy/value.h"

#include "internal/check.h"
#include "internal/reference.h"


namespace worthy {


namespace {


inline bool is_reference_type(Type t) {
    return (static_cast<int>(t) > internal::LastPrimitiveType);
}


} // namespace


Value::Value() : type_{Type::Null} {
    data_.ref = nullptr;
}


Value::Value(bool b) : type_{Type::Boolean} {
    data_.b = b;
}

Value::Value(std::int8_t n) : type_{Type::Int8} {
    data_.i8 = n;
}


Value::Value(std::int16_t n) : type_{Type::Int16} {
    data_.i16 = n;
}


Value::Value(std::int32_t n) : type_{Type::Int32} {
    data_.i32 = n;
}


Value::Value(std::int64_t n) : type_{Type::Int64} {
    data_.i64 = n;
}


Value::Value(std::uint8_t n) : type_{Type::UInt8} {
    data_.u8 = n;
}


Value::Value(std::uint16_t n) : type_{Type::UInt16} {
    data_.u16 = n;
}


Value::Value(std::uint32_t n) : type_{Type::UInt32} {
    data_.u32 = n;
}


Value::Value(std::uint64_t n) : type_{Type::UInt64} {
    data_.u64 = n;
}


Value::Value(float n) : type_{Type::Float} {
    data_.f = n;
}


Value::Value(double n) : type_{Type::Double} {
    data_.d = n;
}


Value::Value(internal::Reference* ref, Type t) : type_{t} {
    WORTHY_CHECK(ref && is_reference_type(t));

    // We steal the reference here.
    data_.ref = ref;
}


Value::Value(const Value& other) : data_{other.data_}, type_{other.type_} {
    retain();
}


Value::Value(Value&& other) : type_{Type::Null} {
    data_.ref = nullptr;
    swap(other);
}


Value::~Value() {
    release();
}


Value& Value::operator=(const Value& other) {
    Value copy(other);
    swap(copy);
    return *this;
}


Value& Value::operator=(Value&& other) {
    swap(other);
    return *this;
}


const internal::Object* Value::object() const {
    WORTHY_CHECK(is_reference_type(type_));
    return data_.ref->object();
}


void Value::retain() {
    if (is_reference_type(type_)) {
        data_.ref->retain();
    }
}


void Value::release() {
    if (is_reference_type(type_)) {
        data_.ref->release();
    }
}


} // namespace worthy
