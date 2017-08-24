#include "worthy/abstract-value.h"

#include "adapters.h"
#include "internal/check.h"
#include "internal/heap.h"
#include "internal/object.h"
#include "internal/variant.h"

#include <ostream>
#include <utility>


using worthy::internal::Object;
using worthy::internal::Variant;


namespace worthy {


namespace {


inline bool isReferenceType(Type t) {
    return (t >= Type::FirstReferenceType);
}


} // namespace


AbstractValue::AbstractValue(Type t, Object* obj)
    : data_{obj},
      type_{t} {
    WORTHY_DCHECK(isReferenceType(t));
    WORTHY_DCHECK(obj);

    intrusive_ptr_add_ref(obj);
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
    if (this != &other) {
        release();
        data_ = other.data_;
        type_ = other.type_;
        retain();
    }
    return *this;
}


AbstractValue& AbstractValue::operator=(AbstractValue&& other) {
    if (this != &other) {
        release();
        data_ = other.data_;
        type_ = other.type_;
        other.reset();
    }
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
        return Object::equals(data_.obj, other.data_.obj);
    }
}


void AbstractValue::reset() {
    data_.obj = nullptr;
    type_ = Type::Null;
}


void AbstractValue::retain() {
    if (isReferenceType(type_)) {
        intrusive_ptr_add_ref(data_.obj);
    }
}


void AbstractValue::release() {
    if (isReferenceType(type_)) {
        intrusive_ptr_release(data_.obj);
    }
}


Object* AbstractValue::object() const {
    WORTHY_DCHECK(isReferenceType(type_));
    return data_.obj;
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
        return Variant(value.data_.obj);
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
        os << value.data_.obj;
        break;
    }
    return os;
}


} // namespace worthy
