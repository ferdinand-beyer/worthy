cmake_minimum_required(VERSION 3.0)

project(Worthy VERSION 0.1.0)


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
    "src/internal/config.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/src-gen/internal/config.h")


add_library(boost INTERFACE)
target_include_directories(boost INTERFACE ${Boost_INCLUDE_DIRS})


add_library(murmur3 STATIC
    third-party/murmur3/MurmurHash3.cpp)
target_include_directories(murmur3 INTERFACE third-party/murmur3)


add_library(catch INTERFACE)
target_include_directories(catch INTERFACE third-party/catch)


add_library(worthy SHARED
    src/internal/block.cpp
    src/internal/block_allocator.cpp
    src/internal/block_owner.cpp
    src/internal/check.cpp
    src/internal/eternity.cpp
    src/internal/frame.cpp
    src/internal/garbage_collector.cpp
    src/internal/gc_worker.cpp
    src/internal/gc_workspace.cpp
    src/internal/generation.cpp
    src/internal/handle.cpp
    src/internal/handle_pool.cpp
    src/internal/hash.cpp
    src/internal/hashmap.cpp
    src/internal/heap.cpp
    src/internal/iterator.cpp
    src/internal/map_iterator.cpp
    src/internal/nursery.cpp
    src/internal/object.cpp
    src/internal/owner_block_allocator.cpp
    src/internal/root_block_allocator.cpp
    src/internal/space.cpp
    src/internal/variant.cpp

    src/abstract_value.cpp
    src/map.cpp
    src/runtime.cpp
    src/string.cpp
    src/utils.cpp
    )
target_include_directories(worthy INTERFACE include)
target_include_directories(worthy PRIVATE
    include src ${CMAKE_CURRENT_BINARY_DIR}/src-gen)
target_link_libraries(worthy boost murmur3)
#set_target_properties(worthy PROPERTIES
#    VERSION ${Worthy_VERSION}
#    SOVERSION 1)


add_executable(worthy-test
    test/block_test.cpp
    test/blocked_vector_test.cpp
    test/garbage_collector_test.cpp
    test/handle_pool_test.cpp
    test/hash_test.cpp
    test/hashmap_test.cpp
    test/map_test.cpp
    test/root_block_allocator_test.cpp
    test/value_test.cpp
    test/variant_test.cpp

    test/main.cpp
    )
target_compile_definitions(worthy-test PRIVATE WORTHY_ENABLE_DEBUG_CHECKS=1)
target_include_directories(worthy-test PRIVATE test src)
target_link_libraries(worthy-test worthy boost catch)

enable_testing()
add_test(NAME worthy-test COMMAND worthy-test)


if(DOXYGEN_FOUND)
    set(DOXYGEN_CONFIG_FILE ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    configure_file(doc/Doxyfile.in ${DOXYGEN_CONFIG_FILE} @ONLY)

    add_custom_target(doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_CONFIG_FILE}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM)
endif()
