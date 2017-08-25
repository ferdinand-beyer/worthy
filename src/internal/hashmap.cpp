#include "internal/hashmap.h"

#include "internal/heap.h"
#include "internal/object-dispatch.h"

#include <bitset>


namespace worthy {
namespace internal {


namespace {


inline uint mask(HashCode hash, uint shift) {
    return (hash >> shift) & 0x1f;
}


inline uint32_t bitpos(HashCode hash, uint shift) {
    return 1 << mask(hash, shift);
}


inline uint bitcount(uint32_t bitmap) {
    return std::bitset<32>(bitmap).count();
}


inline HashMapBitmapNode* emptyBitmapNode(const Object* caller) {
    return caller->heap()->emptyHashMapBitmapNode();
}


inline HashMap* newHashMap(const Object* caller, ElementCount count,
                           const HashMapNode* root, bool has_null_key,
                           const Variant& null_value) {
    return caller->heap()->make<HashMap>(
            count, root, has_null_key, null_value);
}


inline HashMapBitmapNode* newBitmapNode(const Object* caller,
                                        size_t array_length,
                                        uint32_t bitmap) {
    return caller->heap()->makeExtra<HashMapBitmapNode>(
        VariantArray::sizeFor(array_length), bitmap);
}


HashMapNode* createNode(const Object* caller, uint shift,
                        const Variant& key1, const Variant& val1,
                        HashCode hash2,
                        const Variant& key2, const Variant& val2) {
    const auto hash1 = hash(key1);
    if (hash1 == hash2) {
        // TODO: new HashMapCollisionNode
        WORTHY_UNIMPLEMENTED();
    }
    bool added_leaf = false;
    return emptyBitmapNode(caller)
        ->_add(shift, hash1, key1, val1, added_leaf)
        ->add(shift, hash2, key2, val2, added_leaf);
}


} // namespace


// ---------------------------------------------------------------------
// HashMap


HashMap::HashMap()
    : root_{nullptr},
      null_value_{},
      has_null_key_{false},
      count_{0} {
}


HashMap::HashMap(ElementCount count,
                 const HashMapNode* root,
                 bool has_null_key,
                 const Variant& null_value)
    : root_{root},
      null_value_{null_value},
      has_null_key_{has_null_key},
      count_{count} {
}


bool HashMap::containsKey(const Variant& key) const {
    if (key.isNull()) {
        return has_null_key_;
    }
    if (!root_) {
        return false;
    }
    const Variant not_found(const_cast<HashMap*>(this));
    return root_->find(0, hash(key), key, not_found) != not_found;
}


Variant HashMap::get(const Variant& key, const Variant& not_found) const {
    if (key.isNull()) {
        return has_null_key_ ? null_value_ : not_found;
    }
    return root_ ? root_->find(0, hash(key), key, not_found) : not_found;
}


HashMap* HashMap::add(const Variant& key, const Variant& value) const {
    if (key.isNull()) {
        if (has_null_key_ && value == null_value_) {
            // No change to the map.
            return const_cast<HashMap*>(this);
        }
        return newHashMap(this, has_null_key_ ? count_ : count_ + 1,
                          root_, true, value);
    }

    bool added_leaf = false;
    HashMapNode* new_root = (root_ ? root_ : emptyBitmapNode(this))
        ->add(0, hash(key), key, value, added_leaf);

    if (new_root == root_) {
        // No change to the map.
        return const_cast<HashMap*>(this);
    }

    return newHashMap(this, added_leaf ? count_ + 1 : count_,
                      new_root, has_null_key_, null_value_);
}


// ---------------------------------------------------------------------
// HashMapNode


#define NODE_DISPATCH(method, args) \
    DISPATCH_CONST(WORTHY_HASHMAPNODE_DERIVED_TYPES, method, args)


HashMapNode* HashMapNode::add(uint shift, HashCode hash,
                              const Variant& key, const Variant& value,
                              bool& added_leaf) const {
    NODE_DISPATCH(_add, (shift, hash, key, value, added_leaf));
}


Variant HashMapNode::find(uint shift,
                          HashCode hash,
                          const Variant& key,
                          const Variant& not_found) const {
    NODE_DISPATCH(_find, (shift, hash, key, not_found));
}


// ---------------------------------------------------------------------
// HashMapArrayNode


HashMapNode* HashMapArrayNode::_add(uint shift, HashCode hash,
                                    const Variant& key, const Variant& value,
                                    bool& added_leaf) const {
    // TODO
    WORTHY_UNIMPLEMENTED();
}


Variant HashMapArrayNode::_find(uint shift,
                                HashCode hash,
                                const Variant& key,
                                const Variant& not_found) const {
    // TODO
    WORTHY_UNIMPLEMENTED();
}


// ---------------------------------------------------------------------
// HashMapBitmapNode


HashMapBitmapNode::HashMapBitmapNode()
    : bitmap_{0} {
}


HashMapBitmapNode::HashMapBitmapNode(uint32_t bitmap)
    : bitmap_{bitmap} {
}


uint HashMapBitmapNode::count() const {
    return bitcount(bitmap_);
}


uint HashMapBitmapNode::index(uint32_t bit) const {
    return bitcount(bitmap_ & (bit - 1));
}


VariantArray HashMapBitmapNode::array() const {
    return {const_cast<HashMapBitmapNode*>(this + 1),
            static_cast<size_t>(2 * count())};
}


HashMapNode* HashMapBitmapNode::_add(uint shift, HashCode hash,
                                     const Variant& key, const Variant& value,
                                     bool& added_leaf) const {
    const auto bit = bitpos(hash, shift);
    const auto idx = index(bit);

    const auto arr = array();

    if (bitmap_ & bit) {
        // We already have an element at this position.
        auto key_or_null = arr.get(2*idx);
        auto val_or_node = arr.get(2*idx + 1);

        if (key_or_null.isNull()) {
            // Null key means the value is a node.
            WORTHY_DCHECK(val_or_node.isObject());

            auto child = static_cast<HashMapNode*>(val_or_node.toObject());
            auto new_child = child->add(shift + 5, hash, key, value, added_leaf);

            if (child == new_child) {
                return const_cast<HashMapBitmapNode*>(this);
            }

            auto new_node = newBitmapNode(this, arr.length(), bitmap_);
            auto new_array = new_node->array();

            // Replace with new node.
            new_array.copy(arr);
            new_array.set(2*idx + 1, new_child);

            return new_node;
        }

        if (key_or_null == key) {
            if (val_or_node == value) {
                // Entry is already present.
                return const_cast<HashMapBitmapNode*>(this);
            }

            auto new_node = newBitmapNode(this, arr.length(), bitmap_);
            auto new_array = new_node->array();

            // Replace with new value.
            new_array.copy(arr);
            new_array.set(2*idx + 1, value);

            return new_node;
        }

        added_leaf = true;

        auto new_node = newBitmapNode(this, arr.length(), bitmap_);
        auto new_array = new_node->array();

        new_array.copy(arr);
        new_array.set(2*idx, nullptr);
        new_array.set(2*idx + 1, createNode(this, shift + 5,
                                            key_or_null, val_or_node,
                                            hash, key, value));

        return new_node;
    }

    const auto n = count();

    if (n >= 16) {
        // TODO: Switch to ArrayNode
        //WORTHY_UNIMPLEMENTED();
    }

    auto new_node = newBitmapNode(this, 2 * (n+1), bitmap_ | bit);
    auto new_array = new_node->array();

    new_array.copy(0, arr, 0, 2*idx);
    new_array.set(2*idx, key);
    new_array.set(2*idx+1, value);
    new_array.copy(2*(idx+1), arr, 2*idx, 2*(n-idx));

    added_leaf = true;

    return new_node;
}


Variant HashMapBitmapNode::_find(uint shift,
                                 HashCode hash,
                                 const Variant& key,
                                 const Variant& not_found) const {
    const auto bit = bitpos(hash, shift);
    if ((bitmap_ & bit) == 0) {
        return not_found;
    }

    const auto arr = array();
    const auto idx = index(bit);

    const auto key_or_null = arr.get(2*idx);
    const auto val_or_node = arr.get(2*idx+1);

    if (key_or_null.isNull()) {
        const auto node = static_cast<HashMapNode*>(val_or_node.toObject());
        return node->find(shift + 5, hash, key, not_found);
    }

    if (key_or_null == key) {
        return val_or_node;
    }

    return not_found;
}


// ---------------------------------------------------------------------
// HashMapCollisionNode


HashMapNode* HashMapCollisionNode::_add(uint shift,
                                        HashCode hash,
                                        const Variant& key,
                                        const Variant& value,
                                        bool& added_leaf) const {
    // TODO
    WORTHY_UNIMPLEMENTED();
}


Variant HashMapCollisionNode::_find(uint shift,
                                    HashCode hash,
                                    const Variant& key,
                                    const Variant& not_found) const {
    // TODO
    WORTHY_UNIMPLEMENTED();
}


} } // namespace worthy::internal
