#ifndef WORTHY_VALUE_H
#define WORTHY_VALUE_H

#include <utility>

#include "worthy/type.h"

namespace worthy {
namespace internal {

union ValueData {
    char c;
    int i;
    void* ptr;
};

} // namespace internal

class Value {
public:
    Value();
    Value(const Value& other);
    Value(Value&& other);

    ~Value();

    Value& operator=(const Value& other);
    Value& operator=(Value&& other);

    inline void swap(Value& other) {
        using std::swap;
        swap(data_, other.data_);
    }

    inline Type type() const {
        return type_;
    }

protected:
    Value(Type t);

    friend inline void swap(Value& lhs, Value& rhs) {
        lhs.swap(rhs);
    }

private:
    void incRef();
    void decRef();

    internal::ValueData data_;
    Type type_;
};

} // namespace worthy

#endif // WORTHY_VALUE_H
