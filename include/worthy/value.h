#ifndef WORTHY_VALUE_H
#define WORTHY_VALUE_H


#include "worthy/internal/variant-base.h"
#include "worthy/type.h"

#include <utility>


namespace worthy {


namespace internal {
class Object;
class Reference;
}


class Value {
public:
    // Construct a Null value.
    Value();

    Value(bool b);

    Value(std::int8_t n);
    Value(std::int16_t n);
    Value(std::int32_t n);
    Value(std::int64_t n);

    Value(std::uint8_t n);
    Value(std::uint16_t n);
    Value(std::uint32_t n);
    Value(std::uint64_t n);

    Value(float n);
    Value(double n);

    Value(const Value& other);
    Value(Value&& other);

    ~Value();

    Value& operator=(const Value& other);
    Value& operator=(Value&& other);

    inline void swap(Value& other) {
        using std::swap;
        swap(data_, other.data_);
        swap(type_, other.type_);
    }

    inline Type type() const {
        return type_;
    }

protected:
    Value(internal::Reference* ref, Type t);

    const internal::Object* object() const;

    friend inline void swap(Value& lhs, Value& rhs) {
        lhs.swap(rhs);
    }

private:
    void use();
    void release();

    internal::VariantData data_;
    Type type_;
};


} // namespace worthy


#endif // WORTHY_VALUE_H
