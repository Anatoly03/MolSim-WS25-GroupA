# Add yaml-cpp dependency
# https://github.com/jbeder/yaml-cpp

include(FetchContent)

FetchContent_Declare(yaml-cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    # Beware! Version 8 is not tagged as `yaml-cpp-0.8.0`
    GIT_TAG 0.8.0)
FetchContent_MakeAvailable(yaml-cpp)

target_link_libraries(MolSimLib PUBLIC yaml-cpp::yaml-cpp)
target_link_libraries(MolSimTest yaml-cpp::yaml-cpp)
