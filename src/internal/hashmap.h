#ifndef WORTHY_INTERNAL_HASHMAP_H_
#define WORTHY_INTERNAL_HASHMAP_H_


#include "internal/map_iterator.h"
#include "internal/object.h"
#include "internal/object_decl.h"
#include "internal/variant.h"

#include <array>
#include <functional>


namespace worthy {
namespace internal {


typedef std::function<
    bool(const Variant& key, const Variant& value)> KeyValuePredicate;


class HashMapNode : public Object {
public:
    Variant find(uint shift, HashCode hash,
                 const Variant& key, const Variant& not_found) const;

    HashMapNode* add(uint shift, HashCode hash,
                     const Variant& key, const Variant& value,
                     bool& added_leaf) const;

    HashMapNode* remove(uint shift, HashCode hash, const Variant& key) const;

    bool iterate_map(KeyValuePredicate pred) const;

protected:
    HashMapNode() = default;
};


class HashMapBitmapNode final : public HashMapNode {
public:
    DECL_CAST(HashMapBitmapNode)

    HashMapBitmapNode();
    explicit HashMapBitmapNode(uint32_t bitmap);

    Variant find_(uint shift, HashCode hash,
                  const Variant& key, const Variant& not_found) const;

    HashMapNode* add_(uint shift, HashCode hash,
                      const Variant& key, const Variant& value,
                      bool& added_leaf) const;

    HashMapNode* remove_(uint shift, HashCode hash, const Variant& key) const;

    bool do_iterate_map(KeyValuePredicate pred) const;

    void traverse_(ObjectVisitor& visitor);

private:
    uint count() const;
    uint index(uint32_t bit) const;

    VariantArray array() const;

    HashMapNode* updateAt(uint index, uint shift, HashCode hash,
                          const Variant& key, const Variant& value,
                          bool& added_leaf) const;

    HashMapNode* removeAt(uint index, uint bit) const;

    HashMapBitmapNode* copyAndSet(uint array_index, const Variant& value) const;

    HashMapArrayNode* toArrayNode(uint shift, uint32_t count) const;

    uint32_t bitmap_;

    friend class HashMapArrayNode;
};


class HashMapArrayNode final : public HashMapNode {
public:
    typedef std::array<HashMapNode*, 32> NodeArray;

    DECL_CAST(HashMapArrayNode)

    explicit HashMapArrayNode(uint8_t count);

    HashMapArrayNode(uint8_t count, const NodeArray& nodes);

    Variant find_(uint shift, HashCode hash,
                  const Variant& key, const Variant& not_found) const;

    HashMapNode* add_(uint shift, HashCode hash,
                      const Variant& key, const Variant& value,
                      bool& added_leaf) const;

    HashMapNode* remove_(uint shift, HashCode hash, const Variant& key) const;

    bool do_iterate_map(KeyValuePredicate pred) const;

    void traverse_(ObjectVisitor& visitor);

private:
    HashMapBitmapNode* toBitmapNode(uint remove_index) const;

    NodeArray nodes_;
    uint8_t count_;
    //TransientTag tag_;

    friend class HashMapBitmapNode;
};


class HashMapCollisionNode final : public HashMapNode {
public:
    DECL_CAST(HashMapCollisionNode)

    Variant find_(uint shift, HashCode hash,
                  const Variant& key, const Variant& not_found) const;

    HashMapNode* add_(uint shift, HashCode hash,
                      const Variant& key, const Variant& value,
                      bool& added_leaf) const;

    HashMapNode* remove_(uint shift, HashCode hash, const Variant& key) const;

    bool do_iterate_map(KeyValuePredicate pred) const;

    void traverse_(ObjectVisitor& visitor);

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

    HashMap* remove(const Variant& key) const;

    bool iterate_map(KeyValuePredicate pred) const;

    bool equals_(const Object* other) const;

    HashCode hashCode_() const;

    void traverse_(ObjectVisitor& visitor);

private:
    const HashMapNode* const root_;
    const Variant null_value_;
    const bool has_null_key_;
    const uint32_t count_;
    mutable HashCode hash_code_;
};


class TransientHashMap final : public HashMap {
public:
    DECL_CAST(TransientHashMap)

private:
    //TransientTag tag_;
};


class HashMapIterator final : public MapIterator {
public:
    Variant key_() const;
    Variant value_() const;
    bool moveNext_();
};


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_HASHMAP_H_
