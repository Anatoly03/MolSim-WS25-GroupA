# GoogleTest Quickstart
# https://google.github.io/googletest/quickstart-cmake.html

cmake_minimum_required(VERSION 3.14)

# collect all test files
# header don't need to be included but this might be necessary for some IDEs
file(GLOB_RECURSE MY_TEST_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/test/*.h"
)

include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

# windows: prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

find_package(GTest REQUIRED)
add_executable(MolSimTest ${MY_TEST_SRC} ${MY_SRC} PRIVATE ${GTEST_INCLUDE_DIRS})
target_link_libraries(MolSimTest PRIVATE GTest::gtest_main ${GTEST_BOTH_LIBRARIES})

include(GoogleTest)
gtest_discover_tests(MolSimTest)

# https://stackoverflow.com/a/736838
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} ${CMAKE_CURRENT_SOURCE_DIR})
