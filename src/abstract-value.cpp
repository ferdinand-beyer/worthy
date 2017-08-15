#include "worthy/abstract-value.h"

#include "internal/check.h"
#include "internal/reference.h"

#include <utility>


namespace worthy {


namespace {


inline bool isReferenceType(Type t) {
    return (static_cast<int>(t) > internal::LastPrimitiveType);
}


} // namespace


AbstractValue::AbstractValue(Type t, internal::Reference* ref)
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
    data_.ref = nullptr;
    type_ = Type::Null;
}


void AbstractValue::retain() {
    if (isReferenceType(type_)) {
        data_.ref->retain();
    }
}


void AbstractValue::release() {
    if (isReferenceType(type_)) {
        data_.ref->release();
    }
}


const internal::Object* AbstractValue::object() const {
    WORTHY_CHECK(isReferenceType(type_));
    return data_.ref->object();
}


} // namespace worthy
