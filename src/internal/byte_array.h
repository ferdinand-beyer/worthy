#ifndef WORTHY_INTERNAL_BYTE_ARRAY_H_
#define WORTHY_INTERNAL_BYTE_ARRAY_H_


#include "internal/object.h"
#include "internal/object_decl.h"


namespace worthy {
namespace internal {


/// \brief An immutable array of bytes.
///
class ByteArray : public Object {
public:
    DECL_CAST(ByteArray)

    static ByteArray* valueOf(const byte* data, size_t size);

    const byte* data() const;
    size_t size() const;

    bool equals_(const Object* other) const;
    HashCode hashCode_() const;

private:
    explicit ByteArray(size_t size);

    const size_t size_;

    friend class Object;
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_BYTE_ARRAY_H_

