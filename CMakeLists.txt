cmake_minimum_required(VERSION 2.8)

project(worthy)

set(Worthy_Library_SRCS
    include/worthy/map.h
    include/worthy/runtime.h
    include/worthy/type.h
    include/worthy/value.h
    include/worthy/worthy.h
    src/internal/block.h
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
    src/internal/root-page.h
    src/internal/root-space.cpp
    src/internal/root-space.h
    src/internal/root.cpp
    src/internal/root.h
    src/internal/space.cpp
    src/internal/space.h
    src/runtime.cpp
    src/map.cpp
    src/string.cpp
    src/value.cpp
    )

set(Worthy_Tests_SRCS
    ${Worthy_Library_SRCS}
    test/hashmap-test.cpp
    test/main.cpp
    test/sample.cpp
    )

if(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=gnu++14")
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src)

add_library(catch INTERFACE)
target_include_directories(catch INTERFACE third-party/catch)

add_executable(worthy-tests ${Worthy_Tests_SRCS})
target_link_libraries(worthy-tests catch)

enable_testing()
add_test(NAME worthy-tests COMMAND worthy-tests)

