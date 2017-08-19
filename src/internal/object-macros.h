#ifndef UNDEF_OBJECT_MACROS


#include "internal/check.h"

#include <boost/preprocessor/empty.hpp>
#include <boost/preprocessor/expand.hpp>
#include <boost/preprocessor/seq.hpp>
#include <boost/preprocessor/tuple.hpp>


#define DECL_CAST(object_type)                                  \
    static inline object_type* cast(Object* obj) {              \
        WORTHY_CHECK(obj->is##object_type());                   \
        return static_cast<object_type*>(obj);                  \
    }                                                           \
    static inline const object_type* cast(const Object* obj) {  \
        WORTHY_CHECK(obj->is##object_type());                   \
        return static_cast<const object_type*>(obj);            \
    }

// ----------------------------------------------------------------------------
// DISPATCH

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

// ----------------------------------------------------------------------------

#else // UNDEF_OBJECT_MACROS


#undef UNDEF_OBJECT_MACROS

#undef DECL_CAST

#undef DISPATCH_CASE
#undef DISPATCH_CASE_APPLY
#undef DISPATCH_EX
#undef DISPATCH_CONST
#undef DISPATCH


#endif // UNDEF_OBJECT_MACROS
