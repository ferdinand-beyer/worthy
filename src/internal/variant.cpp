#include "internal/variant.h"

#include "internal/check.h"
#include "internal/hash.h"
#include "internal/object.h"
#include "internal/object_visitor.h"

#include <algorithm>
#include <cstring>


namespace worthy {
namespace internal {


HashCode hash(const Variant& variant) {
    switch (variant.type()) {
#define WORTHY_TEMP(name, id, type, field)  \
    case VariantType::name:                 \
        return hash(variant.data().field);
    WORTHY_FOR_EACH_VARIANT_TYPE(WORTHY_TEMP)
#undef WORTHY_TEMP
    }
    WORTHY_UNREACHABLE();
}


bool Variant::operator==(const Variant& other) const {
    if (type_ == other.type_) {
        switch (type_) {
#define WORTHY_TEMP(name, id, type, field)              \
        case VariantType::name:                         \
            return data_.field == other.data_.field;
        WORTHY_FOR_EACH_PRIMITIVE_TYPE(WORTHY_TEMP)
#undef WORTHY_TEMP
        case VariantType::Object:
            return Object::equals(data_.obj, other.data_.obj);
        default:
            WORTHY_UNREACHABLE();
        }
    }
    return false;
}


void Variant::traverse(ObjectVisitor& visitor) {
    if (isObject() && data_.obj) {
        visitor.visit(data_.obj);
    }
}


void VariantArray::clear() {
    std::memset(data_array_, 0, sizeFor(length_));
}


void VariantArray::copy(const VariantArray& src) {
    WORTHY_DCHECK(length_ == src.length_);

    std::memcpy(data_array_, src.data_array_, sizeFor(length_));
}


void VariantArray::copy(size_t dst_index, const VariantArray& src,
                        size_t src_index, size_t length) {
    WORTHY_DCHECK(&src != this);

    if (length == 0) {
        return;
    }

    WORTHY_CHECK(src_index + length <= src.length_);
    WORTHY_CHECK(dst_index + length <= length_);

    std::memcpy(data_array_ + dst_index,
                src.data_array_ + src_index,
                length * sizeof(VariantData));

    std::memcpy(type_array_ + dst_index,
                src.type_array_ + src_index,
                length * sizeof(VariantType));
}


void VariantArray::traverse(ObjectVisitor& visitor) {
    for (size_t i = 0; i < length_; i++) {
        if ((type_array_[i] == VariantType::Object) && data_array_[i].obj) {
            visitor.visit(data_array_[i].obj);
        }
    }
}


} } // namespace worthy::internal
