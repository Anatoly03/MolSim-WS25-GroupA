cmake_minimum_required(VERSION 3.10..4)

# Enable C++17. You may raise the standard if you want
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Options
option(ENABLE_VTK_OUTPUT "Enable VTK output support" ON)
option(ENABLE_OPENMP "Enable OpenMP support" ON)

# define project name, version
project(PSEMolDyn VERSION 0.0.1 LANGUAGES CXX)

# let ccmake and cmake-gui offer the default build type options
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug;Release;RelWithDebInfo;MinSizeRel")

# set Release as the default build type if it is not yet set.
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING
            "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif(NOT CMAKE_BUILD_TYPE)

# collect all cpp files
# header don't need to be included but this might be necessary for some IDEs
file(GLOB_RECURSE APP_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/app/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/app/*.h")
file(GLOB_RECURSE CORE_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/core/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/core/*.h")

# create make targets: app executable and core library
add_library(MolSimLib ${CORE_SRC})
add_executable(MolSim ${APP_SRC})
target_link_libraries(MolSim PRIVATE MolSimLib)

target_include_directories(MolSim
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_include_directories(MolSimLib
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# activate all compiler warnings. Clean up your code :P
# depending on the compiler different flags are used
target_compile_options(MolSim
        PRIVATE
            $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic -Werror>
            # disable some spam warnings for icpc...
            $<$<CXX_COMPILER_ID:Intel>:-w3 -wd383,981,1418,1572,2259>
)

target_compile_options(MolSimLib
        PRIVATE
            $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic -Werror>
            # disable some spam warnings for icpc...
            $<$<CXX_COMPILER_ID:Intel>:-w3 -wd383,981,1418,1572,2259>
)

# create clean target to remove data bloat
add_custom_target(clean-vtk COMMAND rm -rf *.xyz *.vtu *.out)

# add additional modules
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)

#enables VTK
if(ENABLE_VTK_OUTPUT)
    target_compile_definitions(MolSim PRIVATE ENABLE_VTK_OUTPUT)
endif()

include(vtk)
include(doxygen)
include(test-google)
include(add-fmt)
include(add-spdlog)
include(add-yaml)
include(add-tracy)
<<<<<<< HEAD
include(openmp)
=======

# OpenMP support
if(ENABLE_OPENMP)
    find_package(OpenMP)
    if(OpenMP_CXX_FOUND)
        message(STATUS "OpenMP found: enabling OpenMP support")
        target_link_libraries(MolSim PRIVATE OpenMP::OpenMP_CXX)
        target_link_libraries(MolSimLib PRIVATE OpenMP::OpenMP_CXX)
        target_compile_definitions(MolSim PRIVATE OPENMP)
        target_compile_definitions(MolSimLib PRIVATE OPENMP)
    else()
        message(STATUS "OpenMP not found on this system")
    endif()
endif()
>>>>>>> 8a40306 (parallelization: initial implementation; version 0)
