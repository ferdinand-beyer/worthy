#include "worthy/abstract-value.h"

#include "adapters.h"
#include "internal/check.h"
#include "internal/heap.h"
#include "internal/objects.h"
#include "internal/variant.h"

#include <ostream>
#include <utility>


using worthy::internal::Object;
using worthy::internal::Reference;
using worthy::internal::Variant;


namespace worthy {


namespace {


inline bool isReferenceType(Type t) {
    return (t >= Type::FirstReferenceType);
}


bool equals(Reference* lhs, Reference* rhs) {
    if (lhs == rhs) {
        return true;
    }
    if (!lhs || !rhs) {
        return false;
    }
    return Object::equals(lhs->get(), rhs->get());
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


bool AbstractValue::equals(const AbstractValue& other) const {
    if (type_ != other.type_) {
        return false;
    }
    switch (type_) {
    case Type::Null:
        return true;

#define WORTHY_TEMP(name, id, type, field)      \
    case Type::name:                            \
        return (data_.field == other.data_.field);
    WORTHY_FOR_EACH_PRIMITIVE_TYPE(WORTHY_TEMP)
#undef WORTHY_TEMP

    default:
        WORTHY_DCHECK(isReferenceType(type_));
        return worthy::equals(data_.ref, other.data_.ref);
    }
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


Object* AbstractValue::object() const {
    WORTHY_DCHECK(isReferenceType(type_));
    return data_.ref->get();
}


Variant toVariant(const AbstractValue& value) {
    switch (value.type_) {
    case Type::Null:
        return Variant();

#define WORTHY_TEMP(name, id, type, field)      \
    case Type::name:                            \
        return Variant(value.data_.field);
    WORTHY_FOR_EACH_PRIMITIVE_TYPE(WORTHY_TEMP)
#undef WORTHY_TEMP

    default:
        WORTHY_DCHECK(isReferenceType(value.type_));
        return Variant(value.data_.ref->get());
    }
}


std::ostream& operator<<(std::ostream& os, const AbstractValue& value) {
    switch (value.type_) {
    case Type::Null:
        break;

#define WORTHY_TEMP(name, id, type, field)  \
    case Type::name:                        \
        os << value.data_.field;            \
        break;
    WORTHY_FOR_EACH_PRIMITIVE_TYPE(WORTHY_TEMP)
#undef WORTHY_TEMP

    default:
        WORTHY_DCHECK(isReferenceType(value.type_));
        // TODO: Delegate to Object!
        os << value.data_.ref->get();
        break;
    }
    return os;
}


} // namespace worthy
