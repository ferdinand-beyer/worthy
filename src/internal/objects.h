#ifndef WORTHY_INTERNAL_OBJECT_TYPES_H_
#define WORTHY_INTERNAL_OBJECT_TYPES_H_


#include "internal/check.h"
#include "internal/globals.h"
#include "internal/macros.h"
#include "internal/variant.h"

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/empty.hpp>
#include <boost/preprocessor/expand.hpp>
#include <boost/preprocessor/seq.hpp>
#include <boost/preprocessor/tuple.hpp>

#include <cstdint>


namespace worthy {
namespace internal {


class Heap;


const std::size_t BranchBits = 5;
const std::size_t BranchSize = 1 << BranchBits;


#define WORTHY_OBJECT_DERIVED   \
    (Reference)                 \
    (HashMap)                   \
    (TransientHashMap)


#define WORTHY_HASHMAPNODE_DERIVED  \
    (HashMapArrayNode)              \
    (HashMapBitmapNode)             \
    (HashMapCollisionNode)


#define WORTHY_OBJECT_TYPES     \
    WORTHY_OBJECT_DERIVED       \
    WORTHY_HASHMAPNODE_DERIVED


#define DECL_CAST(object_type)                                  \
    static inline object_type* cast(Object* obj) {              \
        WORTHY_CHECK(obj->is##object_type());                   \
        return static_cast<object_type*>(obj);                  \
    }                                                           \
    static inline const object_type* cast(const Object* obj) {  \
        WORTHY_CHECK(obj->is##object_type());                   \
        return static_cast<const object_type*>(obj);            \
    }


#define DISPATCH_CASE(object_type, method, args, const_qualifier)   \
    case ObjectType::object_type:                                   \
        return static_cast<const_qualifier object_type*>(this)      \
            ->method(BOOST_PP_TUPLE_ENUM(args));


#define DISPATCH_CASE_APPLY(r, case_args, object_type)              \
    BOOST_PP_EXPAND(DISPATCH_CASE                                   \
            BOOST_PP_TUPLE_PUSH_FRONT(case_args, object_type))


#define DISPATCH_EX(object_type_seq, method, args, const_qualifier) \
    do {                                                            \
        switch (type()) {                                           \
        BOOST_PP_SEQ_FOR_EACH(                                      \
                DISPATCH_CASE_APPLY,                                \
                (method, args, const_qualifier),                    \
                object_type_seq)                                    \
        default: WORTHY_UNREACHABLE();                              \
        }                                                           \
    } while (false)


#define DISPATCH_CONST(object_type_seq, method, args) \
    DISPATCH_EX(object_type_seq, method, args, const)


#define DISPATCH(object_type_seq, method, args) \
    DISPATCH_EX(object_type_seq, method, args, BOOST_PP_EMPTY())


enum class ObjectType : std::uint8_t {
    BOOST_PP_SEQ_ENUM(WORTHY_OBJECT_TYPES)
};


class Object {
public:
    static bool equals(const Object* a, const Object* b);

    ObjectType type() const;

#define WORTHY_TEMP(r, _, object_type) \
    bool BOOST_PP_CAT(is, object_type)() const;
    BOOST_PP_SEQ_FOR_EACH(WORTHY_TEMP, _, WORTHY_OBJECT_TYPES)
#undef WORTHY_TEMP

    Heap* heap() const;

    HashCode hashCode() const;

    bool equals(const Object* other) const;

protected:
    Object(ObjectType type);

private:
    HashCode _hashCode() const;
    bool _equals(const Object* other) const;

    const ObjectType type_;
    std::uint8_t flags_;
    PageMarker page_marker_;
    std::uint32_t control_;

    friend class Space;
};


class Reference : public Object {
public:
    DECL_CAST(Reference)

    explicit Reference(Object* ptr);

    Object* get();
    const Object* get() const;

    void retain();
    void release();

private:
    WORTHY_DISABLE_COPY(Reference);

    Object* ptr_;

    friend class ReferenceSpace;
};


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

    HashMap(ElementCount count, const HashMapNode* root,
            bool has_null_key, const Variant& null_value);

    ElementCount count() const;

    bool containsKey(const Variant& key) const;

    Variant get(const Variant& key) const;

    HashMap* assoc(const Variant& key, const Variant& value) const;

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
    TransientTag tag_;
};


// ---------------------------------------------------------------------


inline bool Object::equals(const Object* a, const Object* b) {
    return a ? a->equals(b) : !b;
}


inline ObjectType Object::type() const {
    return type_;
}


#define WORTHY_TEMP(r, _, object_type)                          \
inline bool BOOST_PP_CAT(Object::is, object_type)() const {     \
    return (type() == ObjectType::object_type);                 \
}
    BOOST_PP_SEQ_FOR_EACH(WORTHY_TEMP, _, WORTHY_OBJECT_TYPES)
#undef WORTHY_TEMP


// ---------------------------------------------------------------------


inline Object* Reference::get() {
    return ptr_;
}


inline const Object* Reference::get() const {
    return ptr_;
}


// ---------------------------------------------------------------------


inline HashMapNode* HashMapNode::assoc(int shift, HashCode hash,
        const Variant& key, const Variant& value, bool* added_leaf) {
    DISPATCH(WORTHY_HASHMAPNODE_DERIVED, _assoc,
            (shift, hash, key, value, added_leaf));
}


// ---------------------------------------------------------------------


inline ElementCount HashMap::count() const {
    return count_;
}


} } // namespace worthy::internal


#endif // WORTHY_INTERNAL_OBJECT_TYPES_H_
