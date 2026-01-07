# Performance profiling build configuration
# To build with gprof profiling:
#   cmake -DCMAKE_BUILD_TYPE=Release -DENABLE_GPROF=ON ..
#   make
#   ./MolSim [args]
#   gprof MolSim gmon.out > analysis.txt

option(ENABLE_GPROF "Enable gprof profiling (-pg flag)" OFF)

if(ENABLE_GPROF)
    message(STATUS "Enabling gprof profiling support")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
endif()
