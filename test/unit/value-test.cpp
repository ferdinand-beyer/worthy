#include "worthy/value.h"
#include "worthy/type.h"

#include <catch.hpp>


using worthy::Value;
using worthy::Type;


TEST_CASE("construct primitive types", "[value]") {
    SECTION("null") {
        Value val;
        REQUIRE(val.type() == Type::Null);
    }
    SECTION("bool") {
        Value val(true);
        REQUIRE(val.type() == Type::Boolean);
    }
    SECTION("int8") {
        const std::int8_t N = 7;
        Value val(N);
        REQUIRE(val.type() == Type::Int8);
    }
    SECTION("int16") {
        const std::int16_t N = 7;
        Value val(N);
        REQUIRE(val.type() == Type::Int16);
    }
    SECTION("int32") {
        const std::int32_t N = 7;
        Value val(N);
        REQUIRE(val.type() == Type::Int32);
    }
    SECTION("int64") {
        const std::int64_t N = 7;
        Value val(N);
        REQUIRE(val.type() == Type::Int64);
    }
    SECTION("uint8") {
        const std::uint8_t N = 7;
        Value val(N);
        REQUIRE(val.type() == Type::UInt8);
    }
    SECTION("uint16") {
        const std::uint16_t N = 7;
        Value val(N);
        REQUIRE(val.type() == Type::UInt16);
    }
    SECTION("uint32") {
        const std::uint32_t N = 7;
        Value val(N);
        REQUIRE(val.type() == Type::UInt32);
    }
    SECTION("uint64") {
        const std::uint64_t N = 7;
        Value val(N);
        REQUIRE(val.type() == Type::UInt64);
    }
    SECTION("float") {
        Value val(3.1415f);
        REQUIRE(val.type() == Type::Float);
    }
    SECTION("double") {
        Value val(3.1415);
        REQUIRE(val.type() == Type::Double);
    }
}
