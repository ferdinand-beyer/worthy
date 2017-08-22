#ifndef WORTHY_INTERNAL_OBJECT_DISPATCH_H_
#define WORTHY_INTERNAL_OBJECT_DISPATCH_H_


#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/empty.hpp>
#include <boost/preprocessor/expand.hpp>
#include <boost/preprocessor/seq.hpp>
#include <boost/preprocessor/tuple.hpp>


#define CALL_METHOD(object_ptr, method, args) \
    (object_ptr)->method(BOOST_PP_TUPLE_ENUM(args))


#define CALL_FUNCTION(object_ptr, function, args) \
    function((object_ptr), BOOST_PP_TUPLE_ENUM(args))


#define DISPATCH_CASE(object_type, call_macro, object_ptr, name, args,      \
                      const_qualifier)                                      \
    case ObjectType::object_type:                                           \
        return call_macro(                                                  \
                    static_cast<const_qualifier object_type*>(object_ptr),  \
                    name, args);


#define DISPATCH_CASE_APPLY(s, args, object_type)                           \
    BOOST_PP_EXPAND(DISPATCH_CASE                                           \
                    BOOST_PP_TUPLE_PUSH_FRONT(args, object_type))


#define DISPATCH_EX(call_macro, object_ptr, name, args, const_qualifier,    \
                    object_type_seq)                                        \
    do {                                                                    \
        switch ((object_ptr)->type()) {                                     \
        BOOST_PP_SEQ_FOR_EACH(                                              \
                DISPATCH_CASE_APPLY,                                        \
                (call_macro, object_ptr, name, args, const_qualifier),      \
                object_type_seq)                                            \
        default:                                                            \
            WORTHY_UNREACHABLE();                                           \
        }                                                                   \
    } while (false)


#define DISPATCH_CONST(object_type_seq, method, args)   \
    DISPATCH_EX(CALL_METHOD, this, method, args, const, \
                object_type_seq)


#define DISPATCH(object_type_seq, method, args)                     \
    DISPATCH_EX(CALL_METHOD, this, method, args, BOOST_PP_EMPTY(),  \
                object_type_seq)


#endif // WORTHY_INTERNAL_OBJECT_DISPATCH_H_
