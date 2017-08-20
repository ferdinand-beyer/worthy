#include "internal/hashmap.h"

#include "internal/heap.h"

#include <bitset>


namespace worthy {
namespace internal {


namespace {


inline HashMap* newHashMap(const Object* caller, ElementCount count,
                           const HashMapNode* root, bool has_null_key,
                           const Variant& null_value) {
    return caller->heap()->newObject<HashMap>(
            count, root, has_null_key, null_value);
}


inline HashMapBitmapNode* emptyBitmapNode(const Object* caller) {
    return caller->heap()->emptyHashMapBitmapNode();
}


inline std::uint8_t mask(HashCode hash, std::uint8_t shift) {
    return (hash >> shift) & 0x1f;
}


inline std::uint8_t bitpos(HashCode hash, std::uint8_t shift) {
    return 1 << mask(hash, shift);
}


inline std::uint8_t bitcount(std::uint32_t bitmap) {
    return std::bitset<32>(bitmap).count();
}


} // namespace


// ---------------------------------------------------------------------
// HashMap


HashMap::HashMap()
    : Object(ObjectType::HashMap),
      root_{nullptr},
      null_value_{},
      has_null_key_{false},
      count_{0} {
}


HashMap::HashMap(ElementCount count,
                 const HashMapNode* root,
                 bool has_null_key,
                 const Variant& null_value)
    : Object{ObjectType::HashMap},
      root_{root},
      null_value_{null_value},
      has_null_key_{has_null_key},
      count_{count} {
}


bool HashMap::containsKey(const Variant& key) const {
    // TODO
    return true;
}


Variant HashMap::get(const Variant& key) const {
    // TODO
    return null_value_;
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
        ->add(0, hash(key), key, value, &added_leaf);

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
    DISPATCH_CONST(WORTHY_HASHMAPNODE_DERIVED, method, args)


HashMapNode::HashMapNode(ObjectType type)
    : Object{type} {
}


HashMapNode* HashMapNode::add(std::uint8_t shift, HashCode hash,
                              const Variant& key, const Variant& value,
                              bool* added_leaf) const {
    NODE_DISPATCH(_add, (shift, hash, key, value, added_leaf));
}


// ---------------------------------------------------------------------
// HashMapArrayNode


HashMapNode* HashMapArrayNode::_add(std::uint8_t shift, HashCode hash,
                                    const Variant& key, const Variant& value,
                                    bool* added_leaf) const {
    // TODO
    WORTHY_UNIMPLEMENTED();
}


// ---------------------------------------------------------------------
// HashMapBitmapNode


HashMapBitmapNode::HashMapBitmapNode()
    : HashMapNode{ObjectType::HashMapBitmapNode},
      bitmap_{0},
      tag_{0} {
}


std::uint8_t HashMapBitmapNode::index(std::uint32_t bit) const {
    return bitcount(bitmap_ & (bit - 1));
}


VariantArray HashMapBitmapNode::array() const {
    return {reinterpret_cast<Address>(
                const_cast<HashMapBitmapNode*>(this + 1)),
            static_cast<std::size_t>(2 * bitcount(bitmap_))};
}


Variant HashMapBitmapNode::keyAt(std::uint8_t index) const {
    return array().get(2*index);
}


Variant HashMapBitmapNode::valueAt(std::uint8_t index) const {
    return array().get(2*index + 1);
}


HashMapNode* HashMapBitmapNode::_add(std::uint8_t shift, HashCode hash,
                                     const Variant& key, const Variant& value,
                                     bool* added_leaf) const {
    const auto bit = bitpos(hash, shift);
    const auto idx = index(bit);

    if (bitmap_ & bit) {
        // We already have an element at this position.
        auto key = keyAt(idx);
        auto value = valueAt(idx);

        if (key.isNull()) {
            // Null key means the value is a node.
            WORTHY_DCHECK(value.isObject());

            auto node = static_cast<HashMapNode*>(value.toObject());
            auto new_node = node->add(shift + 5, hash, key, value, added_leaf);

            if (node == new_node) {
                return const_cast<HashMapBitmapNode*>(this);
            }

            // TODO: Replace value with new_node
            WORTHY_UNIMPLEMENTED();
        }

        // TODO: Equal keys?
        // TODO: Create a new node.
        WORTHY_UNIMPLEMENTED();
    }

    const auto n = bitcount(bitmap_);

    if (n >= 16) {
        // TODO: Switch to ArrayNode
        WORTHY_UNIMPLEMENTED();
    }

    WORTHY_UNIMPLEMENTED();
}


// ---------------------------------------------------------------------
// HashMapCollisionNode


HashMapNode* HashMapCollisionNode::_add(std::uint8_t shift, HashCode hash,
                                        const Variant& key, const Variant& value,
                                        bool* added_leaf) const {
    // TODO
    WORTHY_UNIMPLEMENTED();
}


} } // namespace worthy::internal
