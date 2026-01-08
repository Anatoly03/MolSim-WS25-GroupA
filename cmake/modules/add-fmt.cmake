# Add yaml-cpp dependency
# https://github.com/jbeder/yaml-cpp

include(FetchContent)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 7.1.3  # or a version compatible with spdlog v1.9.2
)
FetchContent_MakeAvailable(fmt)

target_link_libraries(MolSimLib PUBLIC fmt::fmt)
target_link_libraries(MolSimTest fmt::fmt)
