#include "internal/object.h"
#include "internal/reference.h"
#include "internal/reference-space.h"
#include "internal/check.h"

#include <algorithm>
#include <catch.hpp>


using worthy::internal::Object;
using worthy::internal::ObjectType;
using worthy::internal::Reference;
using worthy::internal::ReferenceSpace;


namespace {


class MockObject : public Object {
public:
    MockObject() : Object{ObjectType::HashMap} {}
};


} // namespace


TEST_CASE("can allocate Reference objects", "[reference]") {
    MockObject obj;

    ReferenceSpace space(nullptr);

    Reference* ref = space.newReference(&obj);

    REQUIRE(ref != nullptr);
    REQUIRE(ref->get() == &obj);
    REQUIRE(space.refCount(ref) == 1);

    SECTION("created reference is owned by the space") {
        REQUIRE(ReferenceSpace::spaceOf(ref) == &space);
    }

    SECTION("multiple references are contiguous") {
        MockObject obj2;

        Reference* ref2 = space.newReference(&obj2);

        REQUIRE((ref + 1) == ref2);
    }
}


TEST_CASE("released references are reused", "[reference]") {
    MockObject arr[10];

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
                    CHECK(refs[i]->get() == (arr + i));
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
    MockObject obj;
    Object* const sentinel = &obj;

    ReferenceSpace space(nullptr, 16);

    Object* ptr = sentinel;

    for (int i = 0; i < 256; ++i) {
        Reference* ref = space.newReference(ptr);
        REQUIRE(ref != nullptr);
        ptr = ref;
    }

    for (int i = 0; i < 256; ++i) {
        Reference* ref = reinterpret_cast<Reference*>(ptr);
        ptr = ref->get();
        ref->release();
    }

    REQUIRE(ptr == sentinel);
}

