option(ENABLE_DOXYGEN "Enable Doxygen documentation generation" OFF)

if(ENABLE_DOXYGEN)
    message(STATUS "Doxygen output enabled")
    find_package(Doxygen REQUIRED)

    if(DOXYGEN_FOUND)
        message (STATUS "Found Doxygen Version: ${DOXYGEN_VERSION}")
    else ()
        message(FATAL_ERROR "Doxygen not found")
    endif ()

    if(DOXYGEN_VERSION VERSION_GREATER_EQUAL 8.9)
         include_directories(${DOXYGEN_INCLUDE_DIRS})
    else()
        include(${DOXYGEN_USE_FILE})
    endif ()

    target_link_libraries(MolSim
            PRIVATE
            ${DOXYGEN_LIBRARIES}
    )
endif()

add_custom_target(doc_doxygen COMMAND doxygen Doxyfile)
