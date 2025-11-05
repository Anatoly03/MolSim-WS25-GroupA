# Add spdlog dependency
# https://stackoverflow.com/a/70844074

Include(FetchContent)

FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 7.1.3  # or a version compatible with spdlog v1.9.2
)

FetchContent_MakeAvailable(fmt)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.9.2
)
set(SPDLOG_HEADER_ONLY ON)
FetchContent_MakeAvailable(spdlog)


add_library(MolSimLib ${MY_SRC})
target_include_directories(MolSimLib PUBLIC ${spdlog_SOURCE_DIR}/include)

target_link_libraries(MolSim PRIVATE spdlog::spdlog fmt::fmt MolSimLib)
target_link_libraries(MolSimTest spdlog::spdlog fmt::fmt MolSimLib)
