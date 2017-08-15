#include "worthy/value.h"
#include "worthy/type.h"

#include <catch.hpp>


using worthy::Value;
using worthy::Type;


TEST_CASE("construct primitive types", "[value]") {
    SECTION("null") {
        Value val;
        REQUIRE(val.type() == Type::Null);
        REQUIRE(val.isNull());
    }

    SECTION("bool") {
        Value val(true);
        REQUIRE(val.type() == Type::Boolean);
        REQUIRE(val.toBoolean() == true);
    }

    SECTION("int8") {
        const std::int8_t N = 7;
        Value val(N);
        REQUIRE(val.type() == Type::Int8);
        REQUIRE(val.toInt8() == 7);
    }

    SECTION("int16") {
        const std::int16_t N = 7;
        Value val(N);
        REQUIRE(val.type() == Type::Int16);
        REQUIRE(val.toInt16() == 7);
    }

    SECTION("int32") {
        const std::int32_t N = 7;
        Value val(N);
        REQUIRE(val.type() == Type::Int32);
        REQUIRE(val.toInt32() == 7);
    }

    SECTION("int64") {
        const std::int64_t N = 7;
        Value val(N);
        REQUIRE(val.type() == Type::Int64);
        REQUIRE(val.toInt64() == 7);
    }

    SECTION("uint8") {
        const std::uint8_t N = 7;
        Value val(N);
        REQUIRE(val.type() == Type::UInt8);
        REQUIRE(val.toUInt8() == 7);
    }

    SECTION("uint16") {
        const std::uint16_t N = 7;
        Value val(N);
        REQUIRE(val.type() == Type::UInt16);
        REQUIRE(val.toUInt16() == 7);
    }

    SECTION("uint32") {
        const std::uint32_t N = 7;
        Value val(N);
        REQUIRE(val.type() == Type::UInt32);
        REQUIRE(val.toUInt32() == 7);
    }

    SECTION("uint64") {
        const std::uint64_t N = 7;
        Value val(N);
        REQUIRE(val.type() == Type::UInt64);
        REQUIRE(val.toUInt64() == 7);
    }

    SECTION("float") {
        Value val(3.1415f);
        REQUIRE(val.type() == Type::Float);
        REQUIRE(val.toFloat() == 3.1415f);
    }

    SECTION("double") {
        Value val(3.1415);
        REQUIRE(val.type() == Type::Double);
        REQUIRE(val.toDouble() == 3.1415);
    }
}


TEST_CASE("copy and move semantics", "[value]") {
    Value orig(std::int32_t(42));

    SECTION("copy construct") {
        Value copy(orig);

        REQUIRE(orig.type() == Type::Int32);
        REQUIRE(orig.toInt32() == 42);

        REQUIRE(copy.type() == orig.type());
        REQUIRE(copy.toInt32() == orig.toInt32());
    }

    SECTION("copy assignment") {
        Value copy;

        REQUIRE(copy.isNull());

        copy = orig;

        REQUIRE(orig.type() == Type::Int32);
        REQUIRE(orig.toInt32() == 42);

        REQUIRE(copy.type() == orig.type());
        REQUIRE(copy.toInt32() == orig.toInt32());
    }

    SECTION("move construct") {
        Value dest(std::move(orig));

        REQUIRE(orig.isNull());

        REQUIRE(dest.type() == Type::Int32);
        REQUIRE(dest.toInt32() == 42);
    }

    SECTION("move assignment") {
        Value dest;

        REQUIRE(dest.isNull());

        dest = std::move(orig);

        REQUIRE(orig.isNull());

        REQUIRE(dest.type() == Type::Int32);
        REQUIRE(dest.toInt32() == 42);
    }
}
