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
public:
    HashMapNode* assoc(int shift, HashCode hash,
            const Variant& key, const Variant& value, bool* added_leaf);

protected:
    HashMapNode() = default;
};


class HashMapArrayNode final : public HashMapNode {
public:
    DECL_CAST(HashMapArrayNode)

    HashMapNode* _assoc(int shift, HashCode hash,
            const Variant& key, const Variant& value, bool* added_leaf);

private:
    std::uint8_t count_;
    TransientTag tag_;
    HashMapNode* nodes_[BranchSize];
};


class HashMapBitmapNode final : public HashMapNode {
public:
    DECL_CAST(HashMapBitmapNode)

    HashMapNode* _assoc(int shift, HashCode hash,
            const Variant& key, const Variant& value, bool* added_leaf);

private:
    std::uint8_t bitmap_;
    TransientTag tag_;
    // VariantType types_
    // VariantData values_
};


class HashMapCollisionNode final : public HashMapNode {
public:
    DECL_CAST(HashMapCollisionNode)

    HashMapNode* _assoc(int shift, HashCode hash,
            const Variant& key, const Variant& value, bool* added_leaf);

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

    const HashMap* assoc(const Variant& key, const Variant& value) const;

private:
    Variant nullValue() const;

    bool has_null_key_;
    VariantType null_type_;
    ElementCount count_;
    VariantData null_data_;
    HashMapNode* root_;
};


class TransientHashMap final : public HashMap {
public:
    DECL_CAST(TransientHashMap)

private:
    TransientTag tag_;
};


inline HashMapNode* HashMapNode::assoc(int shift, HashCode hash,
        const Variant& key, const Variant& value, bool* added_leaf) {
    DISPATCH(WORTHY_HASHMAPNODE_DERIVED, _assoc,
            (shift, hash, key, value, added_leaf));
}


} } // namespace worthy::internal


#define UNDEF_OBJECT_MACROS
#include "internal/object-macros.h"


#endif // WORTHY_INTERNAL_HASHMAP_H_
