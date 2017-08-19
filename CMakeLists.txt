cmake_minimum_required(VERSION 3.0)

project(Worthy VERSION 0.1.0)


set(Worthy_API_VERSION 1)

set(Worthy_SRCS
    include/worthy/internal/primitive.h
    include/worthy/abstract-value.h
    include/worthy/map.h
    include/worthy/runtime.h
    include/worthy/type.h
    include/worthy/value.h
    include/worthy/worthy.h

    src/internal/check.cpp
    src/internal/check.h
    src/internal/globals.h
    src/internal/hash.h
    src/internal/hash.cpp
    src/internal/hashmap.cpp
    src/internal/hashmap.h
    src/internal/heap.cpp
    src/internal/heap.h
    src/internal/object-macros.h
    src/internal/object-space.cpp
    src/internal/object-space.h
    src/internal/object-type.h
    src/internal/object.cpp
    src/internal/object.h
    src/internal/page.cpp
    src/internal/page.h
    src/internal/reference-space.cpp
    src/internal/reference-space.h
    src/internal/reference.cpp
    src/internal/reference.h
    src/internal/space.cpp
    src/internal/space.h
    src/internal/variant.cpp
    src/internal/variant.h

    src/abstract-value.cpp
    src/map.cpp
    src/runtime.cpp
    src/string.cpp
    src/value.cpp
    )

set(UnitTests_SRCS
    test/unit/hash-test.cpp
    test/unit/map-test.cpp
    test/unit/reference-space-test.cpp
    test/unit/value-test.cpp
    test/main.cpp
    )


find_package(Boost 1.54.0 REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})


if(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++14")
endif()

set(Worthy_DEBUG_FLAGS "-Wall -DWORTHY_DEBUG")

set(CMAKE_CXX_FLAGS_DEBUG
    "${CMAKE_CXX_FLAGS_DEBUG} ${Worthy_DEBUG_FLAGS}")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO
    "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${Worthy_DEBUG_FLAGS}")


configure_file(
    "src/internal/version.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/src-gen/internal/version.h")


add_library(murmur3 STATIC
    third-party/murmur3/MurmurHash3.h
    third-party/murmur3/MurmurHash3.cpp)
target_include_directories(murmur3 INTERFACE third-party/murmur3)


add_library(catch INTERFACE)
target_include_directories(catch INTERFACE third-party/catch)


add_library(worthy SHARED ${Worthy_SRCS})
target_include_directories(worthy INTERFACE include)
target_include_directories(worthy PRIVATE
    include src ${CMAKE_CURRENT_BINARY_DIR}/src-gen)
target_link_libraries(worthy murmur3)
#set_target_properties(worthy PROPERTIES
#    VERSION ${Worthy_VERSION}
#    SOVERSION ${Worthy_API_VERSION})


add_executable(unit-tests ${UnitTests_SRCS})
target_include_directories(unit-tests PRIVATE test src)
target_link_libraries(unit-tests worthy catch)
target_compile_definitions(unit-tests PRIVATE WORTHY_ENABLE_DEBUG_CHECKS=1)


enable_testing()
add_test(NAME unit-tests COMMAND unit-tests)

