#include "internal/root.h"
#include "internal/root-space.h"

#include <catch.hpp>

using worthy::internal::Root;
using worthy::internal::RootSpace;

TEST_CASE("Create root objects", "[root-space]") {
    char buffer[8] = {'F', 'o', 'o', 'b', 'a', 'r', '\0'};
    RootSpace space;

    Root* root = space.newRoot(buffer);

    REQUIRE(root != nullptr);
    REQUIRE(root->ptr() == buffer);
    REQUIRE(root->refCount() == 1);
}

