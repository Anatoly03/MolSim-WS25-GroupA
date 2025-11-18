include(FetchContent)

FetchContent_Declare(
  yaml-cpp
  GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
  GIT_TAG master
)
FetchContent_MakeAvailable(yaml-cpp)

target_link_libraries(MolSim PUBLIC yaml-cpp::yaml-cpp)
target_link_libraries(MolSimTest yaml-cpp::yaml-cpp)
