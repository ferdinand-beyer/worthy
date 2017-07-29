cmake_minimum_required(VERSION 2.8)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

project(valbrary)

add_library(catch INTERFACE)
target_include_directories(catch INTERFACE third-party/catch)

set(Tests_SRCS
    test/main.cpp
    test/sample.cpp
    )

add_executable(val-tests ${Tests_SRCS})
target_link_libraries(val-tests catch)

enable_testing()
add_test(NAME val-tests COMMAND val-tests)

