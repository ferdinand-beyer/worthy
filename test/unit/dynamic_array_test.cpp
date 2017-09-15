#include "internal/dynamic_array.h"

#include <catch.hpp>

#include <memory>
#include <string>


using namespace worthy::internal;


TEST_CASE("A default constructed dynamic array", "[dynamic_array]") {
    BlockAllocator allocator;
    DynamicArray<int> array(&allocator);

    SECTION("is empty") {
        REQUIRE(array.empty());
    }

    SECTION("has size zero") {
        REQUIRE(array.size() == 0);
    }

    SECTION("has capacity zero") {
        REQUIRE(array.capacity() == 0);
    }
}


TEST_CASE("Reserving space in a dynamic array", "[dynamic_array]") {
    BlockAllocator allocator;
    DynamicArray<int> array(&allocator);

    array.reserve(100);

    SECTION("increases capacity at least to reserved size") {
        REQUIRE(array.capacity() >= 100);
    }

    SECTION("does nothing if capacity large enough") {
        const auto old_capacity = array.capacity();
        array.reserve(old_capacity);
        REQUIRE(array.capacity() == old_capacity);
    }
}


TEST_CASE("Pushing a value to an empty array", "[dynamic_array]") {
    BlockAllocator allocator;
    DynamicArray<int> array(&allocator);

    array.push_back(42);

    SECTION("makes the array non-empty") {
        REQUIRE_FALSE(array.empty());
    }

    SECTION("increases the size") {
        REQUIRE(array.size() == 1);
    }

    SECTION("increases the capacity") {
        REQUIRE(array.capacity() >= 1);
    }
}


TEST_CASE("Pushing a value to a non-empty array", "[dynamic_array]") {
    BlockAllocator allocator;
    DynamicArray<int> array(&allocator);

    array.push_back(100);

    SECTION("does not change sufficient capacity") {
        array.reserve(2);
        const auto old_capacity = array.capacity();
        array.push_back(200);
        REQUIRE(array.capacity() == old_capacity);
    }

    SECTION("increments size") {
        array.push_back(200);
        REQUIRE(array.size() == 2);

        array.push_back(300);
        REQUIRE(array.size() == 3);
    }

    SECTION("allocates new memory on demand") {
        const auto old_capacity = array.capacity();

        const int n = BlockSize / sizeof(int);
        for (int i = 1; i < n; i++) {
            array.push_back(i);
        }
        REQUIRE(array.capacity() == old_capacity);

        array.push_back(n);
        REQUIRE(array.capacity() > old_capacity);
    }
}


TEST_CASE("Read and write values in dynamic array", "[dynamic_array]") {
    BlockAllocator allocator;
    DynamicArray<int> array(&allocator);
    const DynamicArray<int>& const_array = array;

    const int count = 10 + (BlockSize / sizeof(int));

    for (int i = 1; i <= count; i++) {
        array.push_back(i);
    }

    SECTION("via front()") {
        REQUIRE(array.front() == 1);
        REQUIRE(const_array.front() == 1);

        array.front() = 1337;

        REQUIRE(array.front() == 1337);
        REQUIRE(const_array.front() == 1337);
    }

    SECTION("via back()") {
        REQUIRE(array.back() == count);
        REQUIRE(const_array.back() == count);

        array.back() = 1337;

        REQUIRE(array.back() == 1337);
        REQUIRE(const_array.back() == 1337);
    }

    SECTION("via at()") {
        REQUIRE(array.at(17) == 18);
        REQUIRE(const_array.at(17) == 18);

        array.at(20) = 1337;

        REQUIRE(array.at(20) == 1337);
        REQUIRE(const_array.at(20) == 1337);
    }

    SECTION("via []") {
        REQUIRE(array[41] == 42);
        REQUIRE(const_array[41] == 42);

        array[20] = 1337;

        REQUIRE(array[20] == 1337);
        REQUIRE(const_array[20] == 1337);
    }
}


TEST_CASE("Move value into an array", "[dynamic_array]") {
    BlockAllocator allocator;
    DynamicArray<std::unique_ptr<int>> array(&allocator);

    auto ptr = std::make_unique<int>(42);
    array.push_back(std::move(ptr));

    REQUIRE(array.size() == 1);
    REQUIRE(*array.back() == 42);
}


TEST_CASE("Construct value into an array", "[dynamic_array]") {
    BlockAllocator allocator;
    DynamicArray<std::string> array(&allocator);

    array.emplace_back(5, 'x');

    REQUIRE(array.size() == 1);
    REQUIRE(array.back() == "xxxxx");
}


TEST_CASE("Remove the last value from an array", "[dynamic_array]") {
    BlockAllocator allocator;
    DynamicArray<std::shared_ptr<int>> array(&allocator);

    const auto ptr = std::make_shared<int>(42);

    array.push_back(ptr);
    CHECK(ptr.use_count() == 2);

    array.pop_back();

    REQUIRE(array.empty());
    REQUIRE(ptr.use_count() == 1);
}


TEST_CASE("Clear an array", "[dynamic_array]") {
    BlockAllocator allocator;
    DynamicArray<std::shared_ptr<int>> array(&allocator);

    const auto ptr = std::make_shared<int>(42);
    const int count = 10 + (BlockSize / sizeof(std::shared_ptr<int>));

    for (int i = 0; i < count; i++) {
        array.push_back(ptr);
    }

    REQUIRE(ptr.use_count() == (count + 1));

    array.clear();

    REQUIRE(array.empty());
    REQUIRE(ptr.use_count() == 1);
}


TEST_CASE("Free unused memory", "[dynamic_array]") {
    BlockAllocator allocator;
    DynamicArray<int> array(&allocator);

    const int count = BlockSize / sizeof(int);

    for (int i = 0; i <= count; i++) {
        array.push_back(i);
    }

    const auto old_capacity = array.capacity();

    array.pop_back();

    REQUIRE(array.capacity() == old_capacity);

    array.shrink_to_fit();

    REQUIRE(array.capacity() < old_capacity);
}


TEST_CASE("Iterator traversal", "[dynamic_array]") {
    BlockAllocator allocator;
    DynamicArray<int> array(&allocator);

    const DynamicArray<int>& const_array = array;

    constexpr int n = 3 * (BlockSize / sizeof(int));
    constexpr int expected = (n * (n + 1)) / 2;

    for (int i = 1; i <= n; i++) {
        array.push_back(i);
    }

    int sum = 0;

    SECTION("forwards") {
        for (auto i : array) {
            sum += i;
        }
        REQUIRE(sum == expected);
    }

    SECTION("const forwards") {
        for (auto i : const_array) {
            sum += i;
        }
        REQUIRE(sum == expected);
    }

    SECTION("backwards") {
        for (auto i = array.rbegin(), end = array.rend(); i != end; ++i) {
            sum += *i;
        }
        REQUIRE(sum == expected);
    }

    SECTION("const backwards") {
        for (auto i = const_array.rbegin(), end = const_array.rend();
             i != end; ++i) {
            sum += *i;
        }
        REQUIRE(sum == expected);
    }
}
