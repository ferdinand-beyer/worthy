#include "worthy/abstract-value.h"

#include "internal/check.h"
#include "internal/reference.h"

#include <utility>


using worthy::internal::Reference;


namespace worthy {


namespace {


inline bool isReferenceType(Type t) {
    return (static_cast<int>(t) > internal::LastPrimitiveType);
}


} // namespace


AbstractValue::AbstractValue(Type t, Reference* ref)
    : data_{ref},
      type_{t} {
    WORTHY_DCHECK(isReferenceType(t));
    WORTHY_DCHECK(ref);
}


AbstractValue::AbstractValue(const AbstractValue& other)
    : data_{other.data_},
      type_{other.type_} {
    retain();
}


AbstractValue::AbstractValue(AbstractValue&& other)
    : data_{other.data_},
      type_{other.type_} {
    other.reset();
}


AbstractValue& AbstractValue::operator=(const AbstractValue& other) {
    release();
    data_ = other.data_;
    type_ = other.type_;
    retain();
    return *this;
}


AbstractValue& AbstractValue::operator=(AbstractValue&& other) {
    release();
    data_ = other.data_;
    type_ = other.type_;
    other.reset();
    return *this;
}


AbstractValue::~AbstractValue() {
    release();
}


void AbstractValue::swap(AbstractValue& other) {
    using std::swap;
    swap(data_, other.data_);
    swap(type_, other.type_);
}


void AbstractValue::reset() {
    data_.obj = nullptr;
    type_ = Type::Null;
}


void AbstractValue::retain() {
    if (isReferenceType(type_)) {
        Reference::cast(data_.obj)->retain();
    }
}


void AbstractValue::release() {
    if (isReferenceType(type_)) {
        Reference::cast(data_.obj)->release();
    }
}


bool AbstractValue::toBool() const {
    WORTHY_CHECK(type() == Type::Bool);
    return data_.b;
}


std::int8_t AbstractValue::toInt8() const {
    WORTHY_CHECK(type() == Type::Int8);
    return data_.i8;
}


std::int16_t AbstractValue::toInt16() const {
    WORTHY_CHECK(type() == Type::Int16);
    return data_.i16;
}


std::int32_t AbstractValue::toInt32() const {
    WORTHY_CHECK(type() == Type::Int32);
    return data_.i32;
}


std::int64_t AbstractValue::toInt64() const {
    WORTHY_CHECK(type() == Type::Int64);
    return data_.i64;
}


std::uint8_t AbstractValue::toUInt8() const {
    WORTHY_CHECK(type() == Type::UInt8);
    return data_.u8;
}


std::uint16_t AbstractValue::toUInt16() const {
    WORTHY_CHECK(type() == Type::UInt16);
    return data_.u16;
}


std::uint32_t AbstractValue::toUInt32() const {
    WORTHY_CHECK(type() == Type::UInt32);
    return data_.u32;
}


std::uint64_t AbstractValue::toUInt64() const {
    WORTHY_CHECK(type() == Type::UInt64);
    return data_.u64;
}


float AbstractValue::toFloat() const {
    WORTHY_CHECK(type() == Type::Float32);
    return data_.f32;
}


double AbstractValue::toDouble() const {
    WORTHY_CHECK(type() == Type::Float64);
    return data_.f64;
}


const internal::Object* AbstractValue::object() const {
    WORTHY_CHECK(isReferenceType(type_));
    return internal::Reference::cast(data_.obj)->get();
}


} // namespace worthy
