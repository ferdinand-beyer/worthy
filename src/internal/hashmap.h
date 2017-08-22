#ifndef WORTHY_INTERNAL_HASHMAP_H_
#define WORTHY_INTERNAL_HASHMAP_H_


#include "internal/object.h"
#include "internal/object-decl.h"
#include "internal/variant.h"


namespace worthy {
namespace internal {


class HashMapNode : public Object {
public:
    HashMapNode* add(std::uint8_t shift, HashCode hash,
                     const Variant& key, const Variant& value,
                     bool& added_leaf) const;

    Variant find(std::uint8_t shift, HashCode hash,
                 const Variant& key, const Variant& not_found) const;

protected:
    HashMapNode() = default;
};


class HashMapArrayNode final : public HashMapNode {
public:
    DECL_CAST(HashMapArrayNode)

    HashMapNode* _add(std::uint8_t shift, HashCode hash,
                      const Variant& key, const Variant& value,
                      bool& added_leaf) const;

    Variant _find(std::uint8_t shift, HashCode hash,
                  const Variant& key, const Variant& not_found) const;

private:
    //std::uint8_t count_;
    //TransientTag tag_;
    //HashMapNode* nodes_[32];
};


class HashMapBitmapNode final : public HashMapNode {
public:
    DECL_CAST(HashMapBitmapNode)

    HashMapBitmapNode();

    HashMapNode* _add(std::uint8_t shift, HashCode hash,
                      const Variant& key, const Variant& value,
                      bool& added_leaf) const;

    Variant _find(std::uint8_t shift, HashCode hash,
                  const Variant& key, const Variant& not_found) const;

private:
    std::uint8_t count() const;
    std::uint8_t index(std::uint32_t bit) const;

    VariantArray array() const;

    std::uint32_t bitmap_;
    //TransientTag tag_;
};


class HashMapCollisionNode final : public HashMapNode {
public:
    DECL_CAST(HashMapCollisionNode)

    HashMapNode* _add(std::uint8_t shift, HashCode hash,
                      const Variant& key, const Variant& value,
                      bool& added_leaf) const;

    Variant _find(std::uint8_t shift, HashCode hash,
                  const Variant& key, const Variant& not_found) const;

private:
    //HashCode hash_;
    //TransientTag tag_;
    //ElementCount count_;
};


class HashMap : public Object {
public:
    DECL_CAST(HashMap)

    HashMap();

    HashMap(ElementCount count, const HashMapNode* root,
            bool has_null_key, const Variant& null_value);

    ElementCount count() const;

    bool containsKey(const Variant& key) const;

    Variant get(const Variant& key, const Variant& not_found = Variant()) const;

    HashMap* add(const Variant& key, const Variant& value) const;

private:
    const HashMapNode* const root_;
    const Variant null_value_;
    const bool has_null_key_;
    const ElementCount count_;
};


class TransientHashMap final : public HashMap {
public:
    DECL_CAST(TransientHashMap)

private:
    //TransientTag tag_;
};


inline ElementCount HashMap::count() const {
    return count_;
}


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_HASHMAP_H_
