#ifndef WORTHY_INTERNAL_HASHMAP_H_
#define WORTHY_INTERNAL_HASHMAP_H_


#include "internal/object.h"
#include "internal/object-decl.h"
#include "internal/variant.h"

#include <array>


namespace worthy {
namespace internal {


class HashMapNode : public Object {
public:
    HashMapNode* add(uint shift, HashCode hash,
                     const Variant& key, const Variant& value,
                     bool& added_leaf) const;

    Variant find(uint shift, HashCode hash,
                 const Variant& key, const Variant& not_found) const;

protected:
    HashMapNode() = default;
};


class HashMapBitmapNode final : public HashMapNode {
public:
    DECL_CAST(HashMapBitmapNode)

    HashMapBitmapNode();
    explicit HashMapBitmapNode(uint32_t bitmap);

    HashMapNode* _add(uint shift, HashCode hash,
                      const Variant& key, const Variant& value,
                      bool& added_leaf) const;

    Variant _find(uint shift, HashCode hash,
                  const Variant& key, const Variant& not_found) const;

private:
    uint count() const;
    uint index(uint32_t bit) const;

    VariantArray array() const;

    uint32_t bitmap_;
    //TransientTag tag_;
};


class HashMapArrayNode final : public HashMapNode {
public:
    typedef std::array<HashMapNode*, 32> NodeArray;

    DECL_CAST(HashMapArrayNode)

    explicit HashMapArrayNode(uint32_t count);

    HashMapArrayNode(uint32_t count, const NodeArray& nodes);

    HashMapNode* _add(uint shift, HashCode hash,
                      const Variant& key, const Variant& value,
                      bool& added_leaf) const;

    Variant _find(uint shift, HashCode hash,
                  const Variant& key, const Variant& not_found) const;

private:
    NodeArray nodes_;
    uint32_t count_;
    //TransientTag tag_;

    friend class HashMapBitmapNode;
};


class HashMapCollisionNode final : public HashMapNode {
public:
    DECL_CAST(HashMapCollisionNode)

    HashMapNode* _add(uint shift, HashCode hash,
                      const Variant& key, const Variant& value,
                      bool& added_leaf) const;

    Variant _find(uint shift, HashCode hash,
                  const Variant& key, const Variant& not_found) const;

private:
    //HashCode hash_;
    //TransientTag tag_;
    //uint32_t count_;
};


class HashMap : public Object {
public:
    DECL_CAST(HashMap)

    HashMap();

    HashMap(uint32_t count, const HashMapNode* root,
            bool has_null_key, const Variant& null_value);

    uint32_t count() const;

    bool containsKey(const Variant& key) const;

    Variant get(const Variant& key, const Variant& not_found = Variant()) const;

    HashMap* add(const Variant& key, const Variant& value) const;

private:
    const HashMapNode* const root_;
    const Variant null_value_;
    const bool has_null_key_;
    const uint32_t count_;
};


class TransientHashMap final : public HashMap {
public:
    DECL_CAST(TransientHashMap)

private:
    //TransientTag tag_;
};


inline uint32_t HashMap::count() const {
    return count_;
}


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_HASHMAP_H_
