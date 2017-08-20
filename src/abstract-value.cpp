#include "worthy/abstract-value.h"

#include "internal/check.h"
#include "internal/heap.h"
#include "internal/objects.h"
#include "internal/variant.h"

#include <utility>


namespace worthy {


namespace {


inline bool isReferenceType(Type t) {
    return (t >= Type::FirstReferenceType);
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


internal::Object* AbstractValue::object() const {
    WORTHY_DCHECK(isReferenceType(type_));
    return data_.ref->get();
}


internal::Variant toVariant(const AbstractValue& value) {
    switch (value.type_) {
    case Type::Null:
        return internal::Variant();

#define WORTHY_TEMP(name, id, type, field)      \
    case Type::name:                            \
        return internal::Variant(value.data_.field);
    WORTHY_FOR_EACH_PRIMITIVE_TYPE(WORTHY_TEMP)
#undef WORTHY_TEMP

    default:
        WORTHY_DCHECK(isReferenceType(value.type_));
        return internal::Variant(value.data_.ref->get());
    }
}


} // namespace worthy
