#ifndef WORTHY_INTERNAL_MACROS_H_
#define WORTHY_INTERNAL_MACROS_H_

#define NO_ASSIGN(T) \
    T& operator=(const T&) = delete;

#define NO_COPY(T) \
    T(const T&) = delete; \
    NO_ASSIGN(T)

#endif // WORTHY_INTERNAL_MACROS_H_
