#ifndef WORTHY_INTERNAL_HASHMAP_H_
#define WORTHY_INTERNAL_HASHMAP_H_


#include "internal/object.h"

// Has to be the last include (doesn't have include guards)
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
    DECL_CAST(HashMapArrayNode)

private:
    std::uint8_t count_;
    TransientTag tag_;
    HashMapNode* nodes_[BranchSize];
};


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
