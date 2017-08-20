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


void VariantArray::copy(std::size_t dstIndex, const VariantArray& src,
                        std::size_t srcIndex, std::size_t length) {
    WORTHY_DCHECK(&src != this);

    if (length == 0) {
        return;
    }

    WORTHY_CHECK(srcIndex + length <= src.length_);
    WORTHY_CHECK(dstIndex + length <= length_);

    std::memcpy(data_array_ + dstIndex,
                src.data_array_ + srcIndex,
                length * sizeof(VariantData));

    std::memcpy(type_array_ + dstIndex,
                src.type_array_ + srcIndex,
                length * sizeof(VariantType));
}


} } // namespace worthy::internal
