#include "internal/variant.h"

#include "internal/check.h"
#include "internal/hash.h"
#include "internal/objects.h"

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


void VariantArray::copyTo(VariantArray& dst, std::size_t dstIndex,
                          std::size_t srcIndex, std::size_t length) {
    WORTHY_DCHECK(&dst != this);

    if (length == 0) {
        return;
    }

    WORTHY_CHECK(srcIndex + length <= length_);
    WORTHY_CHECK(dstIndex + length <= dst.length_);

    std::memcpy(dst.data_array_ + dstIndex,
                data_array_ + srcIndex,
                length * sizeof(VariantData));

    std::memcpy(dst.type_array_ + dstIndex,
                type_array_ + srcIndex,
                length * sizeof(VariantType));
}


} } // namespace worthy::internal
