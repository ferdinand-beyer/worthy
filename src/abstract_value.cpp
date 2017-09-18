#include "worthy/abstract_value.h"

#include "internal/check.h"
#include "internal/handle.h"
#include "internal/heap.h"
#include "internal/object.h"
#include "internal/variant.h"

#include <ostream>
#include <utility>


using worthy::internal::Handle;
using worthy::internal::Object;
using worthy::internal::Variant;


namespace worthy {


namespace {


inline constexpr bool isObjectType(Type t) noexcept {
    return (t >= Type::FirstReferenceType);
}


inline Handle* makeHandle(Object* obj) {
    WORTHY_DCHECK(obj);
    return obj->heap()->makeHandle(obj).detach();
}


} // namespace


AbstractValue::AbstractValue(Type t, Handle* handle)
    : data_{handle},
      type_{t} {
    WORTHY_DCHECK(isObjectType(t));
    WORTHY_DCHECK(handle);

    // NOTE: The handle is expected to have an elevated reference count,
    // as returned from HandlePtr.detach().
}


AbstractValue::AbstractValue(Type t, Object* object)
    : data_{makeHandle(object)},
      type_{t} {
    WORTHY_DCHECK(isObjectType(t));
}


AbstractValue::AbstractValue(const AbstractValue& other)
    : data_{other.data_},
      type_{other.type_} {
    retain();
}


AbstractValue::AbstractValue(AbstractValue&& other) noexcept
    : data_{other.data_},
      type_{other.type_} {
    other.reset();
}


AbstractValue::~AbstractValue() {
    release();
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


void AbstractValue::swap(AbstractValue& other) noexcept {
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
        return Object::equals(object(), other.object());
    }
}


void AbstractValue::reset() noexcept {
    data_.handle = nullptr;
    type_ = Type::Null;
}


void AbstractValue::retain() {
    if (isObjectType(type_)) {
        data_.handle->retain();
    }
}


void AbstractValue::release() {
    if (isObjectType(type_)) {
        data_.handle->release();
    }
}


Object* AbstractValue::object() const {
    WORTHY_DCHECK(isObjectType(type_));
    return data_.handle->get();
}


Variant toVariant(const AbstractValue& value) noexcept {
    switch (value.type_) {
    case Type::Null:
        return Variant();

#define WORTHY_TEMP(name, id, type, field)      \
    case Type::name:                            \
        return Variant(value.data_.field);
    WORTHY_FOR_EACH_PRIMITIVE_TYPE(WORTHY_TEMP)
#undef WORTHY_TEMP

    default:
        return Variant(value.object());
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
        // TODO: Delegate to Object!
        os << value.object();
        break;
    }
    return os;
}


} // namespace worthy
