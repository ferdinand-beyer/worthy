#ifndef WORTHY_ABSTRACT_VALUE_H
#define WORTHY_ABSTRACT_VALUE_H


#include "worthy/internal/primitive.h"
#include "worthy/type.h"


namespace worthy {


namespace internal {
class Object;
class Reference;
}


class AbstractValue {
protected:
    AbstractValue();

#define WORTHY_TEMP(name, id, type, field) \
    explicit AbstractValue(type field##_);
    WORTHY_FOR_EACH_PRIMITIVE_TYPE(WORTHY_TEMP)
#undef WORTHY_TEMP

    AbstractValue(Type t, internal::Reference* ref);

    AbstractValue(const AbstractValue& other);
    AbstractValue(AbstractValue&& other);

    AbstractValue& operator=(const AbstractValue& other);
    AbstractValue& operator=(AbstractValue&& other);

    ~AbstractValue();

    void swap(AbstractValue& other);

    Type type() const;

    const internal::Object* object() const;

    template <typename T>
    inline T toPrimitive() const {
        switch (type_) {
#define WORTHY_TEMP(name, id, type, field) \
        case Type::name: \
            return data_.field;
        WORTHY_FOR_EACH_PRIMITIVE_TYPE(WORTHY_TEMP)
#undef WORTHY_TEMP
        default:
            return T();
        }
    }

private:
    union Data {
        internal::Reference* ref;
#define WORTHY_TEMP(name, id, type, field) \
        type field;
        WORTHY_FOR_EACH_PRIMITIVE_TYPE(WORTHY_TEMP)
#undef WORTHY_TEMP

        Data() : ref{nullptr} {}
        explicit Data(internal::Reference* ref_) : ref{ref_} {}
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
};


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
