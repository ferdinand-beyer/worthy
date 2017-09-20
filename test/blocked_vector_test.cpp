#include "internal/blocked_vector.h"
#include "internal/root_block_allocator.h"

#include <catch.hpp>

#include <memory>
#include <string>


using namespace worthy::internal;


TEST_CASE("A default constructed blocked vector", "[blocked_vector]") {
    RootBlockAllocator allocator;
    BlockedVector<int> vector(&allocator);

    SECTION("is empty") {
        REQUIRE(vector.empty());
    }

    SECTION("has size zero") {
        REQUIRE(vector.size() == 0);
    }
}


TEST_CASE("Pushing a value to an empty vector", "[blocked_vector]") {
    RootBlockAllocator allocator;
    BlockedVector<int> vector(&allocator);

    vector.push_back(42);

    SECTION("makes the vector non-empty") {
        REQUIRE_FALSE(vector.empty());
    }

    SECTION("increases the size") {
        REQUIRE(vector.size() == 1);
    }
}


TEST_CASE("Pushing a value to a non-empty vector", "[blocked_vector]") {
    RootBlockAllocator allocator;
    BlockedVector<int> vector(&allocator);

    vector.push_back(100);

    SECTION("increments size") {
        vector.push_back(200);
        REQUIRE(vector.size() == 2);

        vector.push_back(300);
        REQUIRE(vector.size() == 3);
    }

    SECTION("allocates new memory on demand") {
        const auto nblocks = allocator.blocksAllocated();

        const int n = BlockSize / sizeof(int);
        for (int i = 1; i < n; i++) {
            vector.push_back(i);
        }
        REQUIRE(allocator.blocksAllocated() == nblocks);

        vector.push_back(n);
        REQUIRE(allocator.blocksAllocated() > nblocks);
    }
}


TEST_CASE("Read and write values in blocked vector", "[blocked_vector]") {
    RootBlockAllocator allocator;
    BlockedVector<int> vector(&allocator);
    const BlockedVector<int>& const_vector = vector;

    const int count = 10 + (BlockSize / sizeof(int));

    for (int i = 1; i <= count; i++) {
        vector.push_back(i);
    }

    SECTION("via front()") {
        REQUIRE(vector.front() == 1);
        REQUIRE(const_vector.front() == 1);

        vector.front() = 1337;

        REQUIRE(vector.front() == 1337);
        REQUIRE(const_vector.front() == 1337);
    }

    SECTION("via back()") {
        REQUIRE(vector.back() == count);
        REQUIRE(const_vector.back() == count);

        vector.back() = 1337;

        REQUIRE(vector.back() == 1337);
        REQUIRE(const_vector.back() == 1337);
    }

    SECTION("via at()") {
        REQUIRE(vector.at(17) == 18);
        REQUIRE(const_vector.at(17) == 18);

        vector.at(20) = 1337;

        REQUIRE(vector.at(20) == 1337);
        REQUIRE(const_vector.at(20) == 1337);
    }

    SECTION("via []") {
        REQUIRE(vector[41] == 42);
        REQUIRE(const_vector[41] == 42);

        vector[20] = 1337;

        REQUIRE(vector[20] == 1337);
        REQUIRE(const_vector[20] == 1337);
    }
}


TEST_CASE("Move value into an vector", "[blocked_vector]") {
    RootBlockAllocator allocator;
    BlockedVector<std::unique_ptr<int>> vector(&allocator);

    auto ptr = std::make_unique<int>(42);
    vector.push_back(std::move(ptr));

    REQUIRE(vector.size() == 1);
    REQUIRE(*vector.back() == 42);
}


TEST_CASE("Construct value into an vector", "[blocked_vector]") {
    RootBlockAllocator allocator;
    BlockedVector<std::string> vector(&allocator);

    vector.emplace_back(5, 'x');

    REQUIRE(vector.size() == 1);
    REQUIRE(vector.back() == "xxxxx");
}


TEST_CASE("Remove the last value from an vector", "[blocked_vector]") {
    RootBlockAllocator allocator;
    BlockedVector<std::shared_ptr<int>> vector(&allocator);

    const auto ptr = std::make_shared<int>(42);

    vector.push_back(ptr);
    CHECK(ptr.use_count() == 2);

    vector.pop_back();

    REQUIRE(vector.empty());
    REQUIRE(ptr.use_count() == 1);
}


TEST_CASE("Clear an vector", "[blocked_vector]") {
    RootBlockAllocator allocator;
    BlockedVector<std::shared_ptr<int>> vector(&allocator);

    const auto ptr = std::make_shared<int>(42);
    const int count = 10 + (BlockSize / sizeof(std::shared_ptr<int>));

    for (int i = 0; i < count; i++) {
        vector.push_back(ptr);
    }

    REQUIRE(ptr.use_count() == (count + 1));

    vector.clear();

    REQUIRE(vector.empty());
    REQUIRE(ptr.use_count() == 1);
}


TEST_CASE("Free unused memory", "[blocked_vector]") {
    RootBlockAllocator allocator;
    BlockedVector<int> vector(&allocator);

    const int count = BlockSize / sizeof(int);

    for (int i = 0; i <= count; i++) {
        vector.push_back(i);
    }

    const auto nblocks = allocator.blocksAllocated();

    vector.pop_back();

    REQUIRE(allocator.blocksAllocated() == nblocks);

    vector.shrink_to_fit();

    REQUIRE(allocator.blocksAllocated() < nblocks);
}


TEST_CASE("Iterators on empty vector", "[blocked_vector]") {
    RootBlockAllocator allocator;
    BlockedVector<int> vector(&allocator);

    REQUIRE(vector.begin() == vector.end());
    REQUIRE(vector.cbegin() == vector.cend());
    REQUIRE(vector.rbegin() == vector.rend());
    REQUIRE(vector.crbegin() == vector.crend());

    const BlockedVector<int>& const_vector = vector;

    REQUIRE(const_vector.begin() == const_vector.end());
    REQUIRE(const_vector.rbegin() == const_vector.rend());
}


TEST_CASE("Iterator traversal", "[blocked_vector]") {
    RootBlockAllocator allocator;
    BlockedVector<int> vector(&allocator);

    const BlockedVector<int>& const_vector = vector;

    constexpr int n = 3 * (BlockSize / sizeof(int));
    constexpr int expected = (n * (n + 1)) / 2;

    for (int i = 1; i <= n; i++) {
        vector.push_back(i);
    }

    int sum = 0;

    SECTION("forwards") {
        for (auto i : vector) {
            sum += i;
        }
        REQUIRE(sum == expected);
    }

    SECTION("const forwards") {
        for (auto i : const_vector) {
            sum += i;
        }
        REQUIRE(sum == expected);
    }

    SECTION("backwards") {
        for (auto i = vector.rbegin(), end = vector.rend(); i != end; ++i) {
            sum += *i;
        }
        REQUIRE(sum == expected);
    }

    SECTION("const backwards") {
        for (auto i = const_vector.rbegin(), end = const_vector.rend();
             i != end; ++i) {
            sum += *i;
        }
        REQUIRE(sum == expected);
    }
}
