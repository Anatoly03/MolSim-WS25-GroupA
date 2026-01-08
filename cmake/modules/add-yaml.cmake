# Add yaml-cpp dependency
# https://github.com/jbeder/yaml-cpp

include(FetchContent)

FetchContent_Declare(yaml-cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    GIT_TAG 0.8.0)

# Download yaml-cpp but don't configure yet
FetchContent_GetProperties(yaml-cpp)
if(NOT yaml-cpp_POPULATED)
    FetchContent_Populate(yaml-cpp)
    
    # Patch the CMakeLists.txt to fix version requirement
    file(READ "${yaml-cpp_SOURCE_DIR}/CMakeLists.txt" YAML_CMAKE_CONTENT)
    string(REGEX REPLACE "cmake_minimum_required\\(VERSION [0-9]+\\.[0-9]+\\)" 
           "cmake_minimum_required(VERSION 3.10)" 
           YAML_CMAKE_CONTENT "${YAML_CMAKE_CONTENT}")
    file(WRITE "${yaml-cpp_SOURCE_DIR}/CMakeLists.txt" "${YAML_CMAKE_CONTENT}")
    
    # Patch emitterutils.cpp to add missing cstdint include
    file(READ "${yaml-cpp_SOURCE_DIR}/src/emitterutils.cpp" EMITTER_CONTENT)
    string(REGEX REPLACE "#include \"yaml-cpp/null.h\"" 
           "#include <cstdint>\n#include \"yaml-cpp/null.h\"" 
           EMITTER_CONTENT "${EMITTER_CONTENT}")
    file(WRITE "${yaml-cpp_SOURCE_DIR}/src/emitterutils.cpp" "${EMITTER_CONTENT}")
    
    # Now add subdirectory
    add_subdirectory(${yaml-cpp_SOURCE_DIR} ${yaml-cpp_BINARY_DIR})
endif()

target_link_libraries(MolSimLib PUBLIC yaml-cpp::yaml-cpp)
target_link_libraries(MolSimTest yaml-cpp::yaml-cpp)
