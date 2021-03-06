#include "internal/hashmap.h"

#include "internal/eternity.h"
#include "internal/frame.h"
#include "internal/hash.h"
#include "internal/heap.h"
#include "internal/object_dispatch.h"
#include "internal/object_visitor.h"

#include <bitset>


namespace worthy {
namespace internal {


namespace {


inline constexpr uint mask(HashCode hash, uint shift) {
    return (hash >> shift) & 0x1f;
}


inline constexpr uint32_t bitpos(HashCode hash, uint shift) {
    return 1 << mask(hash, shift);
}


inline uint bitcount(uint32_t bitmap) {
    return std::bitset<32>(bitmap).count();
}


inline HashMapBitmapNode* emptyBitmapNode() {
    return Frame::current().heap().eternity().emptyHashMapBitmapNode();
}


inline HashMapBitmapNode* newBitmapNode(size_t array_length, uint32_t bitmap) {
    WORTHY_DCHECK(array_length > 0);
    WORTHY_DCHECK((array_length % 2) == 0);
    return constructEx<HashMapBitmapNode>(
            VariantArray::sizeFor(array_length), bitmap);
}


HashMapNode* newNode(uint shift,
                     const Variant& key1, const Variant& val1,
                     HashCode hash2,
                     const Variant& key2, const Variant& val2) {
    const auto hash1 = hash(key1);
    if (hash1 == hash2) {
        // TODO: new HashMapCollisionNode
        WORTHY_UNIMPLEMENTED();
    }
    bool added_leaf = false;
    return emptyBitmapNode()
        ->add_(shift, hash1, key1, val1, added_leaf)
        ->add(shift, hash2, key2, val2, added_leaf);
}


void copyWithoutPair(VariantArray& dest, const VariantArray& src, uint index) {
    WORTHY_DCHECK(src.length() >= 2);
    WORTHY_DCHECK(dest.length() == (src.length() - 2));

    dest.copy(0, src, 0, 2*index);
    dest.copy(2*index, src, 2*(index+1), dest.length() - 2*index);
}


} // namespace


// ---------------------------------------------------------------------
// HashMap


HashMap::HashMap()
    : root_{nullptr},
      null_value_{},
      has_null_key_{false},
      count_{0},
      hash_code_{0} {
}


HashMap::HashMap(uint32_t count,
                 const HashMapNode* root,
                 bool has_null_key,
                 const Variant& null_value)
    : root_{root},
      null_value_{null_value},
      has_null_key_{has_null_key},
      count_{count},
      hash_code_{0} {
}


uint32_t HashMap::count() const {
    return count_;
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
        return construct<HashMap>(has_null_key_ ? count_ : count_ + 1,
                root_, true, value);
    }

    bool added_leaf = false;
    auto new_root = (root_ ? root_ : emptyBitmapNode())
        ->add(0, hash(key), key, value, added_leaf);

    if (new_root == root_) {
        // No change to the map.
        return const_cast<HashMap*>(this);
    }

    return construct<HashMap>(added_leaf ? count_ + 1 : count_,
            new_root, has_null_key_, null_value_);
}


HashMap* HashMap::remove(const Variant& key) const {
    if (key.isNull()) {
        if (!has_null_key_) {
            return const_cast<HashMap*>(this);
        }
        WORTHY_DCHECK(count_ > 0);
        return construct<HashMap>(count_ - 1, root_, false, nullptr);
    }

    if (!root_) {
        return const_cast<HashMap*>(this);
    }

    auto new_root = root_->remove(0, hash(key), key);
    if (new_root == root_) {
        return const_cast<HashMap*>(this);
    }

    return construct<HashMap>(count_ - 1, new_root, has_null_key_, null_value_);
}


bool HashMap::iterate_map(KeyValuePredicate pred) const {
    if (has_null_key_ && !pred(Variant(), null_value_)) {
        return false;
    }
    return root_ ? root_->iterate_map(pred) : true;
}


bool HashMap::equals_(const Object* other) const {
    if (this == other) {
        return true;
    }
    if (!other || !other->isHashMap()) {
        return false;
    }
    const auto m = HashMap::cast(other);
    if (count_ != m->count_) {
        return false;
    }
    const Variant not_found = m;
    return iterate_map([&](const Variant& key, const Variant& value) {
        return (value == m->get(key, not_found));
    });
}


HashCode HashMap::hashCode_() const {
    HashCode hash = hash_code_;
    if (hash == 0) {
        iterate_map([&](const Variant& key, const Variant& value) {
            HashCode entry_hash = 0;
            hashCombine(entry_hash, key);
            hashCombine(entry_hash, value);
            hash += entry_hash;
            return true;
        });
        hashCombine(hash, count());
        hash_code_ = hash;
    }
    return hash;
}


void HashMap::traverse_(ObjectVisitor& visitor) {
    if (has_null_key_) {
        const_cast<Variant&>(null_value_).traverse(visitor);
    }
    if (root_) {
        visitor.visit(const_cast<HashMapNode*&>(root_));
    }
}


// ---------------------------------------------------------------------
// HashMapNode


#define NODE_DISPATCH(method, args) \
    DISPATCH_CONST(WORTHY_HASHMAPNODE_TYPES, method, args)


Variant HashMapNode::find(uint shift,
                          HashCode hash,
                          const Variant& key,
                          const Variant& not_found) const {
    NODE_DISPATCH(find_, (shift, hash, key, not_found));
}


HashMapNode* HashMapNode::add(uint shift, HashCode hash,
                              const Variant& key, const Variant& value,
                              bool& added_leaf) const {
    NODE_DISPATCH(add_, (shift, hash, key, value, added_leaf));
}


HashMapNode* HashMapNode::remove(uint shift, HashCode hash,
                                 const Variant& key) const {
    NODE_DISPATCH(remove_, (shift, hash, key));
}


bool HashMapNode::iterate_map(KeyValuePredicate pred) const {
    NODE_DISPATCH(do_iterate_map, (pred));
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


Variant HashMapBitmapNode::find_(uint shift,
                                 HashCode hash,
                                 const Variant& key,
                                 const Variant& not_found) const {
    const auto bit = bitpos(hash, shift);
    if ((bitmap_ & bit) == 0) {
        return not_found;
    }

    const auto arr = array();
    const auto idx = index(bit);

    const auto found_key = arr.get(2*idx);
    const auto value = arr.get(2*idx + 1);

    if (found_key.isNull()) {
        const auto node = static_cast<HashMapNode*>(value.toObject());
        return node->find(shift + 5, hash, key, not_found);
    }

    if (found_key == key) {
        return value;
    }

    return not_found;
}


HashMapNode* HashMapBitmapNode::add_(uint shift, HashCode hash,
                                     const Variant& key, const Variant& value,
                                     bool& added_leaf) const {
    const auto bit = bitpos(hash, shift);
    const auto idx = index(bit);

    if (bitmap_ & bit) {
        return updateAt(idx, shift, hash, key, value, added_leaf);
    }

    const auto n = count();

    if (n >= 16) {
        auto new_node = toArrayNode(shift, n + 1);
        const auto k = mask(hash, shift);
        new_node->nodes_[k] = emptyBitmapNode()
            ->add_(shift + 5, hash, key, value, added_leaf);
        return new_node;
    }

    const auto arr = array();

    auto new_node = newBitmapNode(2 * (n+1), bitmap_ | bit);
    auto new_array = new_node->array();

    new_array.copy(0, arr, 0, 2*idx);
    new_array.set(2*idx, key);
    new_array.set(2*idx+1, value);
    new_array.copy(2*(idx+1), arr, 2*idx, 2*(n-idx));

    added_leaf = true;

    return new_node;
}


HashMapNode* HashMapBitmapNode::remove_(uint shift, HashCode hash,
                                        const Variant& key) const {
    const auto bit = bitpos(hash, shift);

    if ((bitmap_ & bit) == 0) {
        return const_cast<HashMapBitmapNode*>(this);
    }

    const auto idx = index(bit);
    const auto arr = array();
    const auto found_key = arr.get(2*idx);

    if (found_key.isNull()) {
        const auto child = static_cast<HashMapNode*>(
                arr.get(2*idx + 1).toObject());

        // XXX: If the child contains only one entry after removing, we could
        // place its value here directly.

        auto new_child = child->remove(shift + 5, hash, key);
        if (new_child == child) {
            return const_cast<HashMapBitmapNode*>(this);
        }

        if (new_child) {
            return copyAndSet(2*idx + 1, new_child);
        }

        return removeAt(idx, bit);
    }

    if (found_key == key) {
        return removeAt(idx, bit);
    }

    return const_cast<HashMapBitmapNode*>(this);
}


HashMapNode* HashMapBitmapNode::updateAt(uint index, uint shift, HashCode hash,
                                         const Variant& key, const Variant& value,
                                         bool& added_leaf) const {
    const auto arr = array();

    const auto current_key = arr.get(2*index);
    const auto current_value = arr.get(2*index + 1);

    if (current_key.isNull()) {
        auto child = static_cast<HashMapNode*>(current_value.toObject());
        auto new_child = child->add(shift + 5, hash, key, value, added_leaf);

        if (child == new_child) {
            return const_cast<HashMapBitmapNode*>(this);
        }

        return copyAndSet(2*index + 1, new_child);
    }

    if (current_key == key) {
        if (current_value == value) {
            return const_cast<HashMapBitmapNode*>(this);
        }
        return copyAndSet(2*index + 1, value);
    }

    auto new_node = newBitmapNode(arr.length(), bitmap_);
    auto new_array = new_node->array();

    new_array.copy(arr);
    new_array.set(2*index, nullptr);
    new_array.set(2*index + 1, newNode(shift + 5,
                                       current_key, current_value,
                                       hash, key, value));

    added_leaf = true;

    return new_node;
}


HashMapNode* HashMapBitmapNode::removeAt(uint index, uint bit) const {
    if (bitmap_ == bit) {
        return nullptr;
    }

    const auto arr = array();

    auto new_node = newBitmapNode(arr.length() - 2, bitmap_ ^ bit);
    auto new_array = new_node->array();

    copyWithoutPair(new_array, arr, index);

    return new_node;
}


HashMapBitmapNode* HashMapBitmapNode::copyAndSet(uint array_index,
                                                 const Variant& value) const {
    const auto arr = array();

    auto new_node = newBitmapNode(arr.length(), bitmap_);
    auto new_array = new_node->array();

    new_array.copy(arr);
    new_array.set(array_index, value);

    return new_node;
}


HashMapArrayNode* HashMapBitmapNode::toArrayNode(uint shift,
                                                 uint32_t count) const {
    const auto arr = array();
    const auto empty = emptyBitmapNode();

    auto node = construct<HashMapArrayNode>(count);

    uint k = 0;
    bool added_leaf = true;

    for (uint i = 0; i < 32; ++i) {
        if (((bitmap_ >> i) & 1) == 0) {
            continue;
        }

        const auto key = arr.get(k);
        const auto value = arr.get(k + 1);

        if (key.isNull()) {
            node->nodes_[i] = static_cast<HashMapNode*>(value.toObject());
        } else {
            node->nodes_[i] = empty->add_(shift + 5, hash(key),
                                          key, value, added_leaf);
        }

        k += 2;
    }

    return node;
}


bool HashMapBitmapNode::do_iterate_map(KeyValuePredicate pred) const {
    const auto arr = array();
    for (uint i = 0; i < arr.length(); i += 2) {
        const auto key = arr.get(i);
        const auto value = arr.get(i + 1);

        if (key.isNull()) {
            auto node = static_cast<HashMapNode*>(value.toObject());
            if (!node->iterate_map(pred)) {
                return false;
            }
        } else if (!pred(key, value)) {
            return false;
        }
    }
    return true;
}


void HashMapBitmapNode::traverse_(ObjectVisitor& visitor) {
    array().traverse(visitor);
}


// ---------------------------------------------------------------------
// HashMapArrayNode


HashMapArrayNode::HashMapArrayNode(uint8_t count)
        : count_{count} {
    WORTHY_DCHECK(count_ <= 32);
    nodes_.fill(nullptr);
}


HashMapArrayNode::HashMapArrayNode(uint8_t count, const NodeArray& nodes)
        : nodes_{nodes},
          count_{count} {
    WORTHY_DCHECK(count_ <= 32);
}


Variant HashMapArrayNode::find_(uint shift,
                                HashCode hash,
                                const Variant& key,
                                const Variant& not_found) const {
    const auto idx = mask(hash, shift);
    const auto node = nodes_[idx];
    if (node) {
        return node->find(shift + 5, hash, key, not_found);
    }
    return not_found;
}


HashMapNode* HashMapArrayNode::add_(uint shift, HashCode hash,
                                    const Variant& key, const Variant& value,
                                    bool& added_leaf) const {
    const auto idx = mask(hash, shift);
    const auto child = nodes_[idx];

    if (!child) {
        auto new_node = construct<HashMapArrayNode>(count_ + 1, nodes_);
        new_node->nodes_[idx] = emptyBitmapNode()
            ->add_(shift + 5, hash, key, value, added_leaf);
        return new_node;
    }

    const auto new_child = child->add(shift + 5, hash, key, value, added_leaf);

    if (new_child == child) {
        return const_cast<HashMapArrayNode*>(this);
    }

    auto new_node = construct<HashMapArrayNode>(count_, nodes_);
    new_node->nodes_[idx] = new_child;
    return new_node;
}


HashMapNode* HashMapArrayNode::remove_(uint shift, HashCode hash,
                                       const Variant& key) const {
    const auto idx = mask(hash, shift);
    const auto child = nodes_[idx];

    if (!child) {
        return const_cast<HashMapArrayNode*>(this);
    }

    auto new_child = child->remove(shift + 5, hash, key);

    if (new_child == child) {
        return const_cast<HashMapArrayNode*>(this);
    }

    if (new_child) {
        auto new_node = construct<HashMapArrayNode>(count_, nodes_);
        new_node->nodes_[idx] = new_child;
        return new_node;
    }

    if (count_ <= 8) {
        return toBitmapNode(idx);
    }

    auto new_node = construct<HashMapArrayNode>(count_ - 1, nodes_);
    new_node->nodes_[idx] = nullptr;
    return new_node;
}


HashMapBitmapNode* HashMapArrayNode::toBitmapNode(uint remove_index) const {
    auto node = newBitmapNode(2 * (count_ - 1), 0);
    auto array = node->array();

    // We only fill the values (initial k = 1), zero the memory to treat
    // all keys as null pointers.
    array.clear();
    uint k = 1;

    for (uint i = 0; i < remove_index; ++i) {
        if (nodes_[i]) {
            array.set(k, nodes_[i]);
            node->bitmap_ |= 1 << i;
            k += 2;
        }
    }

    for (uint i = remove_index + 1; i < 32; ++i) {
        if (nodes_[i]) {
            array.set(k, nodes_[i]);
            node->bitmap_ |= 1 << i;
            k += 2;
        }
    }

    return node;
}


bool HashMapArrayNode::do_iterate_map(KeyValuePredicate pred) const {
    for (auto node : nodes_) {
        if (node && !node->iterate_map(pred)) {
            return false;
        }
    }
    return true;
}


void HashMapArrayNode::traverse_(ObjectVisitor& visitor) {
    for (uint i = 0; i < 32; i++) {
        if (nodes_[i]) {
            visitor.visit(nodes_[i]);
        }
    }
}


// ---------------------------------------------------------------------
// HashMapCollisionNode


Variant HashMapCollisionNode::find_(uint shift,
                                    HashCode hash,
                                    const Variant& key,
                                    const Variant& not_found) const {
    WORTHY_UNIMPLEMENTED(); // TODO
}


HashMapNode* HashMapCollisionNode::add_(uint shift,
                                        HashCode hash,
                                        const Variant& key,
                                        const Variant& value,
                                        bool& added_leaf) const {
    WORTHY_UNIMPLEMENTED(); // TODO
}


HashMapNode* HashMapCollisionNode::remove_(uint shift, HashCode hash,
                                           const Variant& key) const {
    WORTHY_UNIMPLEMENTED(); // TODO
}


bool HashMapCollisionNode::do_iterate_map(KeyValuePredicate pred) const {
    WORTHY_UNIMPLEMENTED(); // TODO
}


void HashMapCollisionNode::traverse_(ObjectVisitor& visitor) {
    WORTHY_UNIMPLEMENTED(); // TODO
}


// ---------------------------------------------------------------------
// HashMapIterator


Variant HashMapIterator::key_() const {
    WORTHY_UNIMPLEMENTED(); // TODO
}


Variant HashMapIterator::value_() const {
    WORTHY_UNIMPLEMENTED(); // TODO
}


bool HashMapIterator::moveNext_() {
    WORTHY_UNIMPLEMENTED(); // TODO
}


} } // namespace worthy::internal
