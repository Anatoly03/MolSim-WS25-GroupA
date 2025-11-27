# Find clang-tidy
find_program(CLANG_TIDY "clang-tidy")

if(CLANG_TIDY)
    message(STATUS "Found clang-tidy: ${CLANG_TIDY}")

    # set clang-tidy as static analyzer if found
    set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY} -p build ${CMAKE_CURRENT_SOURCE_DIR}/src/app/MolSim.cpp --quiet)

    # Create tidy target
    add_custom_target(tidy
            COMMAND ${CLANG_TIDY} src/app/MolSim.cpp -p build --fix
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Running clang-tidy (static analysis)"
    )
else()
    message(WARNING "clang-tidy not found - analyzing target not available")
endif()