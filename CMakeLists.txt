cmake_minimum_required(VERSION 2.8)

project(worthy)

set(Worthy_Library_SRCS
    include/worthy/map.h
    include/worthy/runtime.h
    include/worthy/type.h
    include/worthy/value.h
    include/worthy/worthy.h
    src/hashmap.cpp
    src/hashmap.h
    src/heap/heap.cpp
    src/heap/heap.h
    src/heap/page.cpp
    src/heap/page.h
    src/heap/root-page.h
    src/heap/root-space.cpp
    src/heap/root-space.h
    src/heap/root.cpp
    src/heap/root.h
    src/heap/space.cpp
    src/heap/space.h
    src/internal-runtime.h
    src/map.cpp
    src/memory.cpp
    src/memory.h
    src/runtime.cpp
    src/string.cpp
    src/value.cpp
    )

set(Worthy_Tests_SRCS
    ${Worthy_Library_SRCS}
    test/hashmap-test.cpp
    test/main.cpp
    test/sample.cpp
    )

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=gnu++14")
endif()

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/include)

add_library(catch INTERFACE)
target_include_directories(catch INTERFACE third-party/catch)

add_executable(worthy-tests ${Worthy_Tests_SRCS})
target_link_libraries(worthy-tests catch)

enable_testing()
add_test(NAME worthy-tests COMMAND worthy-tests)

