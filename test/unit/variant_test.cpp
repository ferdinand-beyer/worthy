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
    SECTION("representation is compact") {
        REQUIRE(VariantArray::sizeFor(10) < 10 * sizeof(Variant));
    }

    SECTION("access elements") {
        std::vector<byte> buffer;
        buffer.resize(VariantArray::sizeFor(10));

        VariantArray array(buffer.data(), 10);

        for (int i = 0; i < 10; ++i) {
            array.set(i, 100 + i);
        }

        REQUIRE(array.get(0) == 100);
        REQUIRE(array.get(5) == 105);
        REQUIRE(array.get(9) == 109);
    }

    SECTION("clear array") {
        std::vector<byte> buffer(VariantArray::sizeFor(10), 0xff);

        VariantArray array(buffer.data(), 10);

        array.clear();

        for (int i = 0; i < 10; ++i) {
            // Zero is expected to be VariantType::Object here!
            REQUIRE(array.get(i) == nullptr);
        }
    }

    SECTION("copy arrays") {
        std::vector<byte> buffer1(VariantArray::sizeFor(10));
        std::vector<byte> buffer2(VariantArray::sizeFor(10));

        VariantArray array1(buffer1.data(), 10);

        for (int i = 0; i < 10; ++i) {
            array1.set(i, 100 + i);
        }

        VariantArray array2(buffer2.data(), 10);
        array2.copy(array1);

        for (int i = 0; i < 10; ++i) {
            REQUIRE(array2.get(i) == array1.get(i));
        }
    }

    SECTION("copy array slices") {
        std::vector<byte> buffer;
        buffer.resize(VariantArray::sizeFor(20));

        VariantArray array1(buffer.data(), 10);
        VariantArray array2(buffer.data() + VariantArray::sizeFor(10), 10);

        for (int i = 0; i < 10; ++i) {
            array1.set(i, 100 + i);
            array2.set(i, 7);
        }

        array2.copy(3, array1, 1, 5);

        for (int i = 0; i < 3; ++i) {
            REQUIRE(array2.get(i) == 7);
        }

        for (int i = 0; i < 5; ++i) {
            REQUIRE(array1.get(1 + i) == array2.get(3 + i));
        }

        for (int i = 8; i < 10; ++i) {
            REQUIRE(array2.get(i) == 7);
        }
    }
}

