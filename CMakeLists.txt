cmake_minimum_required(VERSION 3.0)

project(Worthy VERSION 0.1.0)


set(Worthy_API_VERSION 1)

set(Worthy_SRCS
    include/worthy/internal/primitive.h
    include/worthy/abstract_value.h
    include/worthy/map.h
    include/worthy/runtime.h
    include/worthy/type.h
    include/worthy/value.h
    include/worthy/worthy.h

    src/internal/align.h
    src/internal/block.cpp
    src/internal/block.h
    src/internal/block_allocator.cpp
    src/internal/block_allocator.h
    src/internal/block_layout.h
    src/internal/block_owner.cpp
    src/internal/block_owner.h
    src/internal/blocked_vector.h
    src/internal/blocked_vector_iterator.h
    src/internal/check.cpp
    src/internal/check.h
    src/internal/globals.h
    src/internal/hash.cpp
    src/internal/hash.h
    src/internal/hashmap.cpp
    src/internal/hashmap.h
    src/internal/heap.cpp
    src/internal/heap.h
    src/internal/object.cpp
    src/internal/object.h
    src/internal/object_decl.h
    src/internal/object_dispatch.h
    src/internal/object_header.cpp
    src/internal/object_header.h
    src/internal/object_space.cpp
    src/internal/object_space.h
    src/internal/object_type.h
    src/internal/page.cpp
    src/internal/page.h
    src/internal/space.cpp
    src/internal/space.h
    src/internal/variant.cpp
    src/internal/variant.h

    src/abstract_value.cpp
    src/adapters.cpp
    src/adapters.h
    src/map.cpp
    src/runtime.cpp
    src/string.cpp
    )

set(UnitTests_SRCS
    test/unit/block_allocator_test.cpp
    test/unit/block_test.cpp
    test/unit/blocked_vector_test.cpp
    test/unit/hash_test.cpp
    test/unit/hashmap_test.cpp
    test/unit/value_test.cpp
    test/unit/variant_test.cpp
    test/main.cpp
    )

set(IntegrationTests_SRCS
    test/integration/map_test.cpp
    test/main.cpp
    )


find_package(Boost 1.54.0 REQUIRED)
find_package(Doxygen)


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


add_library(boost INTERFACE)
target_include_directories(boost INTERFACE ${Boost_INCLUDE_DIRS})


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
target_link_libraries(worthy boost murmur3)
#set_target_properties(worthy PROPERTIES
#    VERSION ${Worthy_VERSION}
#    SOVERSION ${Worthy_API_VERSION})


add_executable(unit-tests ${UnitTests_SRCS})
target_compile_definitions(unit-tests PRIVATE WORTHY_ENABLE_DEBUG_CHECKS=1)
target_include_directories(unit-tests PRIVATE test src)
target_link_libraries(unit-tests worthy boost catch)


add_executable(integration-tests ${IntegrationTests_SRCS})
target_compile_definitions(integration-tests PRIVATE WORTHY_ENABLE_DEBUG_CHECKS=1)
target_include_directories(integration-tests PRIVATE test)
target_link_libraries(integration-tests worthy catch)


enable_testing()
add_test(NAME unit-tests COMMAND unit-tests)
add_test(NAME integration-tests COMMAND integration-tests)


if(DOXYGEN_FOUND)
    set(DOXYGEN_CONFIG_FILE ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    configure_file(doc/Doxyfile.in ${DOXYGEN_CONFIG_FILE} @ONLY)

    add_custom_target(doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_CONFIG_FILE}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM)
endif()
