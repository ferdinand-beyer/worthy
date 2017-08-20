#include "internal/hashmap.h"

#include "internal/heap.h"
#include "internal/objects.h"
#include "internal/variant.h"

#include <bitset>


namespace worthy {
namespace internal {


namespace {


inline HashMapBitmapNode* emptyBitmapNode(const Object* caller) {
    return caller->heap()->emptyHashMapBitmapNode();
}


inline HashMap* newHashMap(const Object* caller, ElementCount count,
                           const HashMapNode* root, bool has_null_key,
                           const Variant& null_value) {
    return caller->heap()->newObject<HashMap>(
            count, root, has_null_key, null_value);
}


inline HashMapBitmapNode* newBitmapNode(const Object* caller,
                                        std::uint8_t array_length) {
    return caller->heap()->newDynamicObject<HashMapBitmapNode>(
        VariantArray::sizeFor(array_length));
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
    if (key.isNull()) {
        return has_null_key_;
    }
    if (!root_) {
        return false;
    }
    const Variant not_found(this);
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
    DISPATCH_CONST(WORTHY_HASHMAPNODE_DERIVED, method, args)


HashMapNode::HashMapNode(ObjectType type)
    : Object{type} {
}


HashMapNode* HashMapNode::add(std::uint8_t shift, HashCode hash,
                              const Variant& key, const Variant& value,
                              bool& added_leaf) const {
    NODE_DISPATCH(_add, (shift, hash, key, value, added_leaf));
}


const Variant& HashMapNode::find(std::uint8_t shift,
                                 HashCode hash,
                                 const Variant& key,
                                 const Variant& not_found) const {
    NODE_DISPATCH(_find, (shift, hash, key, not_found));
}


// ---------------------------------------------------------------------
// HashMapArrayNode


HashMapNode* HashMapArrayNode::_add(std::uint8_t shift, HashCode hash,
                                    const Variant& key, const Variant& value,
                                    bool& added_leaf) const {
    // TODO
    WORTHY_UNIMPLEMENTED();
}


const Variant& HashMapArrayNode::_find(std::uint8_t shift,
                                       HashCode hash,
                                       const Variant& key,
                                       const Variant& not_found) const {
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


std::uint8_t HashMapBitmapNode::count() const {
    return bitcount(bitmap_);
}


std::uint8_t HashMapBitmapNode::index(std::uint32_t bit) const {
    return bitcount(bitmap_ & (bit - 1));
}


VariantArray HashMapBitmapNode::array() const {
    return {reinterpret_cast<Address>(
                const_cast<HashMapBitmapNode*>(this + 1)),
            static_cast<std::size_t>(2 * count())};
}


HashMapNode* HashMapBitmapNode::_add(std::uint8_t shift, HashCode hash,
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

            auto node = static_cast<HashMapNode*>(val_or_node.toObject());
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

    const auto n = count();

    if (n >= 16) {
        // TODO: Switch to ArrayNode
        WORTHY_UNIMPLEMENTED();
    }

    HashMapBitmapNode* new_node = newBitmapNode(this, 2 * (n+1));

    new_node->bitmap_ = bitmap_ | bit;

    auto new_array = new_node->array();

    new_array.copy(0, arr, 0, 2*idx);
    new_array.set(2*idx, key);
    new_array.set(2*idx+1, value);
    new_array.copy(2*(idx+1), arr, 2*idx, 2*(n-idx));

    added_leaf = true;

    return new_node;
}


const Variant& HashMapBitmapNode::_find(std::uint8_t shift,
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


HashMapNode* HashMapCollisionNode::_add(std::uint8_t shift,
                                        HashCode hash,
                                        const Variant& key,
                                        const Variant& value,
                                        bool& added_leaf) const {
    // TODO
    WORTHY_UNIMPLEMENTED();
}


const Variant& HashMapCollisionNode::_find(std::uint8_t shift,
                                           HashCode hash,
                                           const Variant& key,
                                           const Variant& not_found) const {
    // TODO
    WORTHY_UNIMPLEMENTED();
}


} } // namespace worthy::internal
