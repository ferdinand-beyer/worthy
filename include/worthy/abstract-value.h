#ifndef WORTHY_ABSTRACT_VALUE_H
#define WORTHY_ABSTRACT_VALUE_H


#include "worthy/internal/primitive.h"
#include "worthy/type.h"

#include <iosfwd>


namespace worthy {


namespace internal {
class Object;
class Variant;
}


class AbstractValue {
public:
    bool equals(const AbstractValue& other) const;

protected:
    AbstractValue();

#define WORTHY_TEMP(name, id, type, field) \
    explicit AbstractValue(type field##_);
    WORTHY_FOR_EACH_PRIMITIVE_TYPE(WORTHY_TEMP)
#undef WORTHY_TEMP

    AbstractValue(Type t, internal::Object* obj);

    AbstractValue(const AbstractValue& other);
    AbstractValue(AbstractValue&& other);

    AbstractValue& operator=(const AbstractValue& other);
    AbstractValue& operator=(AbstractValue&& other);

    ~AbstractValue();

    void swap(AbstractValue& other);

    Type type() const;

    internal::Object* object() const;

    template <typename T>
    inline T toPrimitive() const {
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
        internal::Object* obj;
#define WORTHY_TEMP(name, id, type, field) \
        type field;
        WORTHY_FOR_EACH_PRIMITIVE_TYPE(WORTHY_TEMP)
#undef WORTHY_TEMP

        Data() : obj{nullptr} {}
        explicit Data(internal::Object* obj_) : obj{obj_} {}
#define WORTHY_TEMP(name, id, type, field) \
        explicit Data(type field##_) : field{field##_} {}
        WORTHY_FOR_EACH_PRIMITIVE_TYPE(WORTHY_TEMP)
#undef WORTHY_TEMP
    };

    void reset();
    void retain();
    void release();

    Data data_;
    Type type_;

    friend internal::Variant toVariant(const AbstractValue& value);
    friend std::ostream& operator<<(std::ostream& os, const AbstractValue& value);
};


inline bool operator==(const AbstractValue& lhs, const AbstractValue& rhs) {
    return lhs.equals(rhs);
}


inline bool operator!=(const AbstractValue& lhs, const AbstractValue& rhs) {
    return !lhs.equals(rhs);
}


inline AbstractValue::AbstractValue()
    : type_{Type::Null} {}


#define WORTHY_TEMP(name, id, type, field) \
inline AbstractValue::AbstractValue(type field##_) \
    : data_{field##_}, type_{Type::name} {}
    WORTHY_FOR_EACH_PRIMITIVE_TYPE(WORTHY_TEMP)
#undef WORTHY_TEMP


inline Type AbstractValue::type() const {
    return type_;
}


} // namespace worthy


#endif // WORTHY_ABSTRACT_VALUE_H
