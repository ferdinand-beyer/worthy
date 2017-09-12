#include "internal/hash.h"

#include <limits>
#include <string>

#include <catch.hpp>


using namespace worthy::internal;


TEST_CASE("hash code for primitive types", "[hash]") {
    SECTION("booleans") {
        REQUIRE(hash(true) == hash(true));
        REQUIRE(hash(false) == hash(false));
        REQUIRE(hash(true) != hash(false));
    }

    SECTION("general integers") {
        REQUIRE(hash(0) == hash(0));
        REQUIRE(hash(1) == hash(1));
        REQUIRE(hash(0) != hash(1));

        REQUIRE(hash(0) == hash(0u));
        REQUIRE(hash(1) == hash(1u));
    }

    SECTION("integers with different bit width") {
        REQUIRE(hash(int8_t(-42)) == hash(int16_t(-42)));
        REQUIRE(hash(int8_t(-42)) == hash(int32_t(-42)));
        REQUIRE(hash(int8_t(-42)) == hash(int64_t(-42)));

        REQUIRE(hash(uint8_t(255)) == hash(uint16_t(255)));
        REQUIRE(hash(uint8_t(255)) == hash(uint32_t(255)));
        REQUIRE(hash(uint8_t(255)) == hash(uint64_t(255)));

        REQUIRE(hash(int8_t(17)) == hash(uint8_t(17)));
        REQUIRE(hash(int16_t(1337)) == hash(uint16_t(1337)));
        REQUIRE(hash(int32_t(1337)) == hash(uint32_t(1337)));
        REQUIRE(hash(int64_t(1337)) == hash(uint64_t(1337)));
    }

    SECTION("floating points") {
        REQUIRE(hash(0.0) == hash(0.0));
        REQUIRE(hash(1.0) == hash(1.0));
        REQUIRE(hash(0.0) != hash(1.0));

        REQUIRE(hash(0.125f) == hash(0.125));

        REQUIRE(hash(-0.0) == hash(0.0));
        REQUIRE(hash(-0.0f) == hash(0.0));
    }

    SECTION("all NaN values hash equally") {
        constexpr auto fqnan = std::numeric_limits<float>::quiet_NaN();
        constexpr auto fsnan = std::numeric_limits<float>::signaling_NaN();
        constexpr auto dqnan = std::numeric_limits<double>::quiet_NaN();
        constexpr auto dsnan = std::numeric_limits<double>::signaling_NaN();

        REQUIRE(hash(fqnan) == hash(dqnan));
        REQUIRE(hash(fsnan) == hash(dsnan));
        REQUIRE(hash(fqnan) == hash(fsnan));
    }
}


TEST_CASE("hash code of known string", "[hash]") {
    std::string str;

    HashCode h = hash(str.data(), str.length());

    REQUIRE(h == 0);

    str = "MurmurHash3";
    h = hash(str.data(), str.length());

    // Gold value taken from http://murmurhash.shorelabs.com/
    REQUIRE(h == 1473193682u);
}
