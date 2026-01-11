# This integrates OpenMP support.

# https://stackoverflow.com/a/48216682
if(APPLE)
    # find homebrew libomp path, store in LIBOMP_PREFIX
    execute_process(COMMAND brew --prefix libomp
                    OUTPUT_VARIABLE LIBOMP_PREFIX
                    OUTPUT_STRIP_TRAILING_WHITESPACE
                    ERROR_QUIET)
    
    if(CMAKE_C_COMPILER_ID MATCHES "Clang")
        set(OpenMP_C "${CMAKE_C_COMPILER}")

        # https://stackoverflow.com/a/60564952
        set(OpenMP_C_FLAGS "-Xpreprocessor -fopenmp")

        # set(OpenMP_C_LIB_NAMES "libomp" "libgomp" "libiomp5")
        # set(OpenMP_libomp_LIBRARY ${OpenMP_C_LIB_NAMES})
        # set(OpenMP_libgomp_LIBRARY ${OpenMP_C_LIB_NAMES})
        # set(OpenMP_libiomp5_LIBRARY ${OpenMP_C_LIB_NAMES})

        # use homebrew libomp if found
        if(LIBOMP_PREFIX)
            set(OpenMP_C_FLAGS "${OpenMP_C_FLAGS} -I${LIBOMP_PREFIX}/include")
            set(OpenMP_C_LIB_NAMES "omp")
            set(OpenMP_omp_LIBRARY "${LIBOMP_PREFIX}/lib/libomp.dylib")
        else()
            set(OpenMP_C_LIB_NAMES "omp")
        endif()
    endif()
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(OpenMP_CXX "${CMAKE_CXX_COMPILER}")

        # https://stackoverflow.com/a/60564952
        set(OpenMP_CXX_FLAGS "-Xpreprocessor -fopenmp")

        # set(OpenMP_CXX_LIB_NAMES "libomp" "libgomp" "libiomp5")
        # set(OpenMP_libomp_LIBRARY ${OpenMP_CXX_LIB_NAMES})
        # set(OpenMP_libgomp_LIBRARY ${OpenMP_CXX_LIB_NAMES})
        # set(OpenMP_libiomp5_LIBRARY ${OpenMP_CXX_LIB_NAMES})

        # use homebrew libomp if found
        if(LIBOMP_PREFIX)
            set(OpenMP_CXX_FLAGS "${OpenMP_CXX_FLAGS} -I${LIBOMP_PREFIX}/include")
            set(OpenMP_CXX_LIB_NAMES "omp")
            set(OpenMP_omp_LIBRARY "${LIBOMP_PREFIX}/lib/libomp.dylib")
        else()
            set(OpenMP_CXX_LIB_NAMES "omp")
        endif()
    endif()
endif()

find_package(OpenMP REQUIRED)

# https://cliutils.gitlab.io/modern-cmake/chapters/packages/OpenMP.html
# https://stackoverflow.com/a/53823820
if(OpenMP_CXX_FOUND)
    target_link_libraries(MolSimLib PUBLIC OpenMP::OpenMP_CXX)
    target_link_libraries(MolSim PUBLIC OpenMP::OpenMP_CXX)

    # https://stackoverflow.com/a/48216682
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")

    message(STATUS "OpenMP found and enabled")
else()
    message(STATUS "OpenMP not found")
endif()
