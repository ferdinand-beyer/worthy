cmake_minimum_required(VERSION 2.8)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

project(worthy)

set(Worthy_Library_SRCS
    include/worthy/worthy.h
    src/string.cpp
    )

set(Worthy_Tests_SRCS
    ${Worthy_Library_SRCS}
    test/main.cpp
    test/sample.cpp
    )

include_directories(include)

add_library(catch INTERFACE)
target_include_directories(catch INTERFACE third-party/catch)

add_executable(worthy-tests ${Worthy_Tests_SRCS})
target_link_libraries(worthy-tests catch)

enable_testing()
add_test(NAME worthy-tests COMMAND worthy-tests)

