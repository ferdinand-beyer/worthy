#ifndef WORTHY_INTERNAL_MACROS_H_
#define WORTHY_INTERNAL_MACROS_H_

// TODO: Use BOOST_LIKELY?
#define WORTHY_LIKELY(x) x
#define WORTHY_UNLIKELY(x) x

#define WORTHY_DISABLE_ASSIGN(T)        \
    T& operator=(const T&) = delete;

#define WORTHY_DISABLE_COPY(T)          \
    T(const T&) = delete;               \
    WORTHY_DISABLE_ASSIGN(T)

#define WORTHY_NOOP ((void) 0)

#endif // WORTHY_INTERNAL_MACROS_H_
