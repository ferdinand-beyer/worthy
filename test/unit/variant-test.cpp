#include "internal/variant.h"

#include <vector>

#include <catch.hpp>


using worthy::internal::Byte;
using worthy::internal::Variant;
using worthy::internal::VariantArray;
using worthy::internal::VariantType;


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
