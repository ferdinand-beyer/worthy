#include "internal/byte_array.h"
#include "internal/heap.h"

#include <catch.hpp>

#include <cstring>


using namespace worthy::internal;


namespace {


struct TestContext {
    Heap heap;

    TestContext() {
        heap.lock();
    }

    ~TestContext() {
        heap.unlock();
    }

    ByteArray* byteArray(const char* s) {
        return ByteArray::valueOf(
                reinterpret_cast<const byte*>(s),
                std::strlen(s));
    }
};


} // namespace


TEST_CASE("An empty ByteArray", "[byte_array]") {
    TestContext context;
    ByteArray* ba = context.byteArray("");

    SECTION("has zero size") {
        REQUIRE(ba->size() == 0);
    }

    SECTION("is null-terminated") {
        REQUIRE(*ba->data() == '\0');
    }
}


TEST_CASE("A non-empty ByteArray", "[byte_array]") {
    const char* const c_string = "The quick brown fox";
    const size_t length = std::strlen(c_string);

    TestContext context;
    ByteArray* ba = context.byteArray(c_string);

    SECTION("has the same size as the original string") {
        REQUIRE(ba->size() == length);
    }

    SECTION("copies the string") {
        REQUIRE(std::memcmp(ba->data(), c_string, length) == 0);
    }

    SECTION("is null-terminated") {
        auto data = ba->data();
        REQUIRE(data[length] == '\0');
    }
}


TEST_CASE("ByteArrays compare equal if their data is equal", "[byte_array]") {
    TestContext context;

    ByteArray* ba1 = context.byteArray("foo");
    ByteArray* ba2 = context.byteArray("foo");
    ByteArray* ba3 = context.byteArray("bar");
    ByteArray* ba4 = context.byteArray("foobar");

    REQUIRE(ba1->equals(ba1));

    REQUIRE_FALSE(ba1->equals(nullptr));

    REQUIRE(ba1->equals(ba2));
    REQUIRE(ba2->equals(ba1));

    REQUIRE_FALSE(ba1->equals(ba3));
    REQUIRE_FALSE(ba3->equals(ba1));

    REQUIRE_FALSE(ba1->equals(ba4));
    REQUIRE_FALSE(ba4->equals(ba1));
}


TEST_CASE("Equal ByteArrays have the same hash code", "[byte_array]") {
    TestContext context;

    ByteArray* ba1 = context.byteArray("foo");
    ByteArray* ba2 = context.byteArray("foo");
    ByteArray* ba3 = context.byteArray("bar");

    REQUIRE(ba1->hashCode() == ba2->hashCode());
    REQUIRE_FALSE(ba1->hashCode() == ba3->hashCode());
}
