# Add spdlog dependency
# https://stackoverflow.com/a/70844074

include(FetchContent)

FetchContent_Declare(spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.9.2)
set(SPDLOG_HEADER_ONLY ON)
FetchContent_MakeAvailable(spdlog)

target_link_libraries(MolSim PUBLIC spdlog::spdlog)
target_link_libraries(MolSimLib PUBLIC spdlog::spdlog)
target_link_libraries(MolSimTest spdlog::spdlog)
