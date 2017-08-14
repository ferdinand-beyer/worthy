#ifndef UNDEF_OBJECT_MACROS


#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/seq/for_each.hpp>


#define DECL_CAST(object_type)                                  \
    static inline object_type* cast(Object* obj) {              \
        WORTHY_CHECK(obj->is##object_type());                   \
        return static_cast<object_type*>(obj);                  \
    }                                                           \
    static inline const object_type* cast(const Object* obj) {  \
        WORTHY_CHECK(obj->is##object_type());                   \
        return static_cast<const object_type*>(obj);            \
    }


#define DECL_IS_TYPE_HELPER(r, _, object_type)                  \
    inline bool BOOST_PP_CAT(is, object_type)() const {         \
        return (type() == ObjectType::object_type);             \
    }


#define DECL_IS_TYPE(object_type_seq) \
    BOOST_PP_SEQ_FOR_EACH(DECL_IS_TYPE_HELPER, _, object_type_seq)


// TODO
#define WORTHY_ASSERT_SIZE(Cls) \
    static_assert(sizeof(Cls) == (Cls::Size + Cls::Padding), \
            "unexpected size of type " #Cls)


#else // UNDEF_OBJECT_MACROS


#undef UNDEF_OBJECT_MACROS

#undef DECL_CAST
#undef DECL_IS_TYPE_HELPER
#undef DECL_IS_TYPE


#endif // UNDEF_OBJECT_MACROS
