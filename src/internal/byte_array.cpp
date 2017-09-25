#include "internal/byte_array.h"

#include "internal/frame.h"
#include "internal/hash.h"

#include <cstring>


namespace worthy {
namespace internal {


ByteArray* ByteArray::valueOf(const byte* data, size_t size) {
    auto self = constructEx<ByteArray>(size + 1, size);
    auto dest = reinterpret_cast<byte*>(self + 1);
    std::memcpy(dest, data, size);
    dest[size] = '\0';
    return self;
}


ByteArray::ByteArray(size_t size) : size_{size} {
}


const byte* ByteArray::data() const {
    return reinterpret_cast<const byte*>(this + 1);
}


size_t ByteArray::size() const {
    return size_;
}


bool ByteArray::equals_(const Object* other) const {
    if (other == this) {
        return true;
    }
    if (!other || !other->isByteArray()) {
        return false;
    }
    const ByteArray* ba = ByteArray::cast(other);
    if (ba->size_ != size_) {
        return false;
    }
    return !std::memcmp(data(), ba->data(), size_);
}


HashCode ByteArray::hashCode_() const {
    return hash(data(), size());
}


} } // namespace worthy::internal
