#ifndef WORTHY_INTERNAL_HASHMAP_H_
#define WORTHY_INTERNAL_HASHMAP_H_


#include "internal/object.h"
#include "internal/object-macros.h"


namespace worthy {
namespace internal {


const std::size_t BranchBits = 5;
const std::size_t BranchSize = 1 << BranchBits;


class HashMapNode : public Object {
protected:
    HashMapNode() = default;
};


class HashMapArrayNode final : public HashMapNode {
public:
    static const std::size_t Size = Object::Size + 1 +
        PointerSize + (BranchSize * PointerSize);
    static const std::size_t Padding = 5;

    DECL_CAST(HashMapArrayNode)

private:
    std::uint8_t count_;
    TransientTag tag_;
    HashMapNode* nodes_[BranchSize];
};


WORTHY_ASSERT_SIZE(HashMapArrayNode);


class HashMapBitmapNode final : public HashMapNode {
public:
    DECL_CAST(HashMapBitmapNode)

private:
    std::uint8_t bitmap_;
    TransientTag tag_;
    // VariantType types_
    // VariantData values_
};


class HashMapCollisionNode final : public HashMapNode {
public:
    DECL_CAST(HashMapCollisionNode)

private:
    HashCode hash_;
    TransientTag tag_;
    ElementCount count_;
    // VariantType types_
    // VariantData values_
};


class HashMap : public Object {
public:
    static const std::size_t Size = Object::Size + 1 + 1 + 4 + 8 + PointerSize;
    static const std::size_t Padding = 0;

    DECL_CAST(HashMap)

    HashMap();

    inline ElementCount count() const {
        return count_;
    }

private:
    bool has_null_key_;
    VariantType null_key_value_type_;
    ElementCount count_;
    VariantData null_key_value_;
    HashMapNode* root_;
};


WORTHY_ASSERT_SIZE(HashMap);


class TransientHashMap final : public HashMap {
public:
    DECL_CAST(TransientHashMap)

private:
    TransientTag tag_;
};


} } // namespace worthy::internal


#define UNDEF_OBJECT_MACROS
#include "internal/object-macros.h"


#endif // WORTHY_INTERNAL_HASHMAP_H_
