cmake_minimum_required(VERSION 2.8)

project(worthy)

set(Worthy_Library_SRCS
    include/worthy/map.h
    include/worthy/runtime.h
    include/worthy/type.h
    include/worthy/value.h
    include/worthy/worthy.h
    src/internal/block.h
    src/internal/check.cpp
    src/internal/check.h
    src/internal/free-block.cpp
    src/internal/free-block.h
    src/internal/hashmap.cpp
    src/internal/hashmap.h
    src/internal/heap.cpp
    src/internal/heap.h
    src/internal/internal-runtime.h
    src/internal/memory.cpp
    src/internal/memory.h
    src/internal/page.cpp
    src/internal/page.h
    src/internal/reference-space.cpp
    src/internal/reference-space.h
    src/internal/reference.cpp
    src/internal/reference.h
    src/internal/space.cpp
    src/internal/space.h
    src/runtime.cpp
    src/map.cpp
    src/string.cpp
    src/value.cpp
    )

set(Worthy_UnitTests_SRCS
    ${Worthy_Library_SRCS}
    test/unit/reference-space-test.cpp
    test/main.cpp
    )

if(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=gnu++14")
endif()

find_package(Boost 1.36.0)
if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

include_directories(include src)

add_library(catch INTERFACE)
target_include_directories(catch INTERFACE third-party/catch)

add_executable(unit-tests ${Worthy_UnitTests_SRCS})
target_include_directories(unit-tests PRIVATE test)
target_link_libraries(unit-tests catch)
target_compile_definitions(unit-tests PRIVATE WORTHY_ENABLE_DEBUG_CHECKS=1)

enable_testing()
add_test(NAME unit-tests COMMAND unit-tests)

