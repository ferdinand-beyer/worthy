#include "internal/reference.h"
#include "internal/reference-space.h"
#include "internal/check.h"

#include <catch.hpp>

using worthy::internal::Reference;
using worthy::internal::ReferenceSpace;

TEST_CASE("can allocate Reference objects", "[reference]") {
    char obj[8] = {'F', 'o', 'o', 'b', 'a', 'r', '\0'};

    ReferenceSpace space(nullptr);

    Reference* ref = space.newReference(obj);

    REQUIRE(ref != nullptr);
    REQUIRE(ref->ptr() == obj);
    REQUIRE(ref->useCount() == 1);

    SECTION("created reference is owned by the space") {
        REQUIRE(space.owns(ref));
    }

    SECTION("multiple references are contiguous") {
        Reference* ref2 = space.newReference(obj);

        REQUIRE((ref + 1) == ref2);
    }
}

