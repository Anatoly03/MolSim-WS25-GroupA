# GoogleTest Quickstart
# https://google.github.io/googletest/quickstart-cmake.html
# https://cmake.org/cmake/help/book/mastering-cmake/chapter/Testing%20With%20CMake%20and%20CTest.html

cmake_minimum_required(VERSION 3.14)

# GoogleTest requires at least C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# collect all test files
# header don't need to be included but this might be necessary for some IDEs
file(GLOB_RECURSE MY_TEST_SRC
        "${PROJECT_SOURCE_DIR}/test/*.cc"
        "${PROJECT_SOURCE_DIR}/test/*.cpp"
        "${PROJECT_SOURCE_DIR}/test/*.h"
)

include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY    https://github.com/google/googletest.git
    GIT_TAG           v1.17.0
)

# windows: prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

enable_testing()
add_executable(MolSimTest ${MY_TEST_SRC})
target_link_libraries(MolSimTest GTest::gtest_main)
add_test(NAME MolSimTest COMMAND MolSimTest)
