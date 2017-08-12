#include "internal/reference.h"
#include "internal/reference-space.h"
#include "internal/check.h"

#include <algorithm>
#include <catch.hpp>

using worthy::internal::Reference;
using worthy::internal::ReferenceSpace;

TEST_CASE("can allocate Reference objects", "[reference]") {
    char obj[8] = {'F', 'o', 'o', 'b', 'a', 'r', '\0'};

    ReferenceSpace space(nullptr);

    Reference* ref = space.newReference(obj);

    REQUIRE(ref != nullptr);
    REQUIRE(ref->ptr() == obj);
    REQUIRE(ref->useCount() == 1);

    SECTION("created reference is owned by the space") {
        REQUIRE(ReferenceSpace::ownerOf(ref) == &space);
    }

    SECTION("multiple references are contiguous") {
        Reference* ref2 = space.newReference(obj + 1);

        REQUIRE((ref + 1) == ref2);
    }
}

TEST_CASE("released references are reused", "[reference]") {
    int arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    ReferenceSpace space(nullptr, 8);

    SECTION("first released is reused when full") {
        Reference* first = space.newReference(arr);
        first->release();

        Reference* last = first;

        // Fill page to capacity without releasing the objects.
        for (int i = 1; i < 8; ++i) {
            Reference* ref = space.newReference(arr + i);
            CHECK(ref > last);
            last = ref;
        }

        // This next allocation should re-use the released first object.
        Reference* next = space.newReference(arr + 8);

        REQUIRE(next == first);
    }

    SECTION("multiple reuses") {
        Reference* refs[8];

        std::fill(refs, refs + 8, nullptr);

        for (int k = 0; k < 3; ++k) {
            for (int n = 8; n > 0; --n) {
                // Create n references.
                for (int i = 0; i < n; ++i) {
                    refs[i] = space.newReference(arr + i);
                    REQUIRE(refs[i] != nullptr);
                }

                // Check all references.
                for (int i = 0; i < n; ++i) {
                    CHECK(refs[i]->ptr() == (arr + i));
                }

                // Release (n - 1) references, leaking one.
                for (int i = 0; i < (n - 1); ++i) {
                    refs[i]->release();
                    refs[i] = nullptr;
                }
            }

            // Release "leaked" references.
            for (int i = 0; i < 8; ++i) {
                refs[i]->release();
                refs[i] = nullptr;
            }
        }
    }
}

TEST_CASE("allocates new pages when out of memory", "[reference]") {
    int obj = 0;
    void* const sentinel = &obj;

    ReferenceSpace space(nullptr, 16);

    void* ptr = sentinel;

    for (int i = 0; i < 256; ++i) {
        Reference* ref = space.newReference(ptr);
        REQUIRE(ref != nullptr);
        ptr = ref;
    }

    for (int i = 0; i < 256; ++i) {
        Reference* ref = reinterpret_cast<Reference*>(ptr);
        ptr = ref->ptr();
        ref->release();
    }

    REQUIRE(ptr == sentinel);
}

