# Add Tracy Profiler dependency
# https://github.com/wolfpld/tracy?tab=readme-ov-file

option(ENABLE_TRACY "Enable Tracy profiler support" OFF)

if(ENABLE_TRACY)
    message(STATUS "Tracy profiler enabled")
    
    include(FetchContent)
    
    FetchContent_Declare(
        tracy
        GIT_REPOSITORY https://github.com/wolfpld/tracy.git
        GIT_TAG v0.11.1
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )
    
    FetchContent_MakeAvailable(tracy)

    target_link_libraries(MolSimLib PUBLIC TracyClient)
    target_link_libraries(MolSim PUBLIC TracyClient)

    target_compile_definitions(MolSimLib PUBLIC TRACY_ENABLE)
    target_compile_definitions(MolSim PUBLIC TRACY_ENABLE)
    
    message(STATUS "Tracy profiler linked successfully")
else()
    message(STATUS "Tracy profiler disabled (use -DENABLE_TRACY=ON to enable)")
endif()
