#include "internal/hash.h"
#include "internal/variant.h"

#include <vector>

#include <catch.hpp>


using namespace worthy::internal;


TEST_CASE("hash code", "[variant]") {
    Variant v1(true);
    Variant v2(17);

    REQUIRE(hash(v1) == hash(true));
    REQUIRE(hash(v2) == hash(17));
}


TEST_CASE("compact array of variant types", "[variant]") {
    REQUIRE(VariantArray::sizeFor(10) < 10 * sizeof(Variant));

    std::vector<Byte> buffer;
    buffer.resize(VariantArray::sizeFor(10));

    VariantArray array(buffer.data(), 10);

    for (int i = 0; i < 10; ++i) {
        array.set(i, 100 + i);
    }

    REQUIRE(array.get(0) == 100);
    REQUIRE(array.get(5) == 105);
    REQUIRE(array.get(9) == 109);
}

