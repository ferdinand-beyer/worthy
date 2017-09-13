#include "internal/hash.h"

#include <string>

#include <catch.hpp>


using worthy::internal::HashCode;
using worthy::internal::hash;


TEST_CASE("hash code for primitive types", "[hash]") {
    SECTION("booleans") {
        REQUIRE(hash(true) == hash(true));
        REQUIRE(hash(false) == hash(false));
        REQUIRE(hash(true) != hash(false));
    }

    SECTION("integers") {
        REQUIRE(hash(0) == hash(0));
        REQUIRE(hash(1) == hash(1));
        REQUIRE(hash(0) != hash(1));
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
