# Find clang-tidy
find_program(CLANG_TIDY "clang-tidy")

if(CLANG_TIDY)
    message(STATUS "Found clang-tidy: ${CLANG_TIDY}")

    # TODO fix clang-tidy
    # set clang-tidy as static analyzer if found
    # set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY} src/MolSim.cpp -p build")

    # Create tidy target
    add_custom_target(tidy
            COMMAND ${CLANG_TIDY} src/MolSim.cpp -p build --fix
            COMMENT "Running clang-tidy (static analysis)"
    )
else()
    message(WARNING "clang-tidy not found - analyzing target not available")
endif()