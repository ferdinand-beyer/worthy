#include "internal/check.h"

#include <cstdarg>
#include <cstdio>
#include <cstdlib>

namespace worthy {
namespace internal {

void fatal(const char* file, int line, const char* format, ...) {
    using namespace std;

    fflush(stdout);
    fflush(stderr);

    fprintf(stderr, "\n\nFatal error in %s, line %d\n", file, line);

    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    fflush(stderr);
    abort();
}

} } // namespace worthy::internal
