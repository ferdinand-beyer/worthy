#include "internal/hash.h"
#include "internal/object.h"

#include <string>

#include <catch.hpp>


using worthy::internal::HashCode;
using worthy::internal::Object;
using worthy::internal::ObjectType;
using worthy::internal::hash;


namespace {


class MockObject : public Object {
public:
    MockObject() : Object{ObjectType::FreeSpace} {}
};


} // namespace


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


TEST_CASE("hash code of an object", "[hash]") {
    MockObject object;

    REQUIRE(hash(&object) == object.hashCode());
}

