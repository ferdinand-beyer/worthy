#ifndef WORTHY_ABSTRACT_VALUE_H
#define WORTHY_ABSTRACT_VALUE_H


#include "worthy/internal/primitive.h"
#include "worthy/type.h"

#include <iosfwd>


namespace worthy {


namespace internal {
class Handle;
class Object;
class Variant;
}


class AbstractValue {
public:
    bool equals(const AbstractValue& other) const;

protected:
    AbstractValue() noexcept;

#define WORTHY_TEMP(name, id, type, field) \
    explicit AbstractValue(type field##_) noexcept;
    WORTHY_FOR_EACH_PRIMITIVE_TYPE(WORTHY_TEMP)
#undef WORTHY_TEMP

    AbstractValue(Type t, internal::Handle* handle);
    AbstractValue(Type t, internal::Object* obj);

    AbstractValue(const AbstractValue& other);
    AbstractValue(AbstractValue&& other) noexcept;

    AbstractValue& operator=(const AbstractValue& other);
    AbstractValue& operator=(AbstractValue&& other);

    ~AbstractValue();

    void swap(AbstractValue& other) noexcept;

    Type type() const noexcept;

    internal::Object* object() const;

    template <typename T>
    inline T toPrimitive() const noexcept {
        switch (type_) {
#define WORTHY_TEMP(name, id, type, field)  \
        case Type::name:                    \
            return data_.field;
        WORTHY_FOR_EACH_PRIMITIVE_TYPE(WORTHY_TEMP)
#undef WORTHY_TEMP
        default:
            return T();
        }
    }

private:
    union Data {
        internal::Handle* handle;
#define WORTHY_TEMP(name, id, type, field) \
        type field;
        WORTHY_FOR_EACH_PRIMITIVE_TYPE(WORTHY_TEMP)
#undef WORTHY_TEMP

        Data() noexcept : handle{nullptr} {}
        explicit Data(internal::Handle* h) noexcept : handle{h} {}
#define WORTHY_TEMP(name, id, type, field) \
        explicit Data(type val) noexcept : field{val} {}
        WORTHY_FOR_EACH_PRIMITIVE_TYPE(WORTHY_TEMP)
#undef WORTHY_TEMP
    };

    void reset() noexcept;

    void retain();
    void release();

    Data data_;
    Type type_;

    friend internal::Variant toVariant(const AbstractValue& value) noexcept;
    friend std::ostream& operator<<(std::ostream& os, const AbstractValue& value);
};


inline bool operator==(const AbstractValue& lhs, const AbstractValue& rhs) {
    return lhs.equals(rhs);
}


inline bool operator!=(const AbstractValue& lhs, const AbstractValue& rhs) {
    return !lhs.equals(rhs);
}


inline AbstractValue::AbstractValue() noexcept
    : type_{Type::Null} {}


#define WORTHY_TEMP(name, id, type, field)                  \
inline AbstractValue::AbstractValue(type field##_) noexcept \
    : data_{field##_}, type_{Type::name} {}
    WORTHY_FOR_EACH_PRIMITIVE_TYPE(WORTHY_TEMP)
#undef WORTHY_TEMP


inline Type AbstractValue::type() const noexcept {
    return type_;
}


} // namespace worthy


#endif // WORTHY_ABSTRACT_VALUE_H
