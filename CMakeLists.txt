cmake_minimum_required(VERSION 2.8.9)

add_subdirectory(core)
add_subdirectory(builder)
add_subdirectory(plugins)

if(CMAKE_ENABLE_DOC)
    find_package(Doxygen)

    if(DOXYGEN_FOUND)
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
        add_custom_target(doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
                          WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                          COMMENT "Generating API documentation with Doxygen" VERBATIM)
    endif(DOXYGEN_FOUND)

    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs/
            DESTINATION ${CMAKE_INSTALL_PREFIX}/docs)
endif(CMAKE_ENABLE_DOC)
