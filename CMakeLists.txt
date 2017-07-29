cmake_minimum_required(VERSION 2.8)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

project(valbrary)

set(Tests_SRCS
    test/catch.hpp
    test/main.cpp
    test/sample.cpp
    )

add_executable(val-tests ${Tests_SRCS})

enable_testing()
add_test(NAME val-tests COMMAND val-tests)

