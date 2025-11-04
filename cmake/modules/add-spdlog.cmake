# Add spdlog dependency
# https://stackoverflow.com/a/70844074

Include(FetchContent)
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.9.2
)
FetchContent_MakeAvailable(spdlog)
target_link_libraries(MolSim PRIVATE spdlog::spdlog)
