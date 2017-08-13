#ifndef WORTHY_INTERNAL_HASHMAP_H_
#define WORTHY_INTERNAL_HASHMAP_H_


#include "internal/object.h"


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

private:
    std::uint8_t count_;
    TransientTag tag_;
    HashMapNode* nodes_[BranchSize];
};


WORTHY_ASSERT_SIZE(HashMapArrayNode);


class HashMapBitmapNode final : public HashMapNode {
private:
    std::uint8_t bitmap_;
    TransientTag tag_;
    // VariantType types_
    // VariantData values_
};


class HashMapCollisionNode final : public HashMapNode {
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

    HashMap();

private:
    bool has_null_key_;
    VariantType null_key_value_type_;
    ElementCount count_;
    VariantData null_key_value_;
    HashMapNode* root_;
};


WORTHY_ASSERT_SIZE(HashMap);


class TransientHashMap final : public HashMap {
private:
    TransientTag tag_;
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_HASHMAP_H_
