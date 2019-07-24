cmake_minimum_required(VERSION 2.8.9)

option(BRAIN_ENABLE_DOUBLE_PRECISION "Enable double precision" OFF)
option(BRAIN_ENABLE_LOGGING          "Enable logging"          OFF)
option(BRAIN_ENABLE_TESTING          "Enable testing"          OFF)
option(BRAIN_ENABLE_DOC              "Enable documentation"    OFF)
option(BRAIN_ENABLE_TRAINER          "Enable trainer building" ON)

if (BRAIN_ENABLE_DOUBLE_PRECISION)
    message(STATUS "Enable DOUBLE precision")
    add_definitions(-DBRAIN_ENABLE_DOUBLE_PRECISION)
endif(BRAIN_ENABLE_DOUBLE_PRECISION)

if (BRAIN_ENABLE_LOGGING)
    message(STATUS "Enable logging")
    add_definitions(-DBRAIN_ENABLE_LOGGING)
endif(BRAIN_ENABLE_LOGGING)

add_compile_options(-O2
                    -g3
                    -Wall
                    -Wextra
                    -Wshadow
                    -Wunused
                    -lm)

add_subdirectory(core)
add_subdirectory(models)
add_subdirectory(iface)
if(BRAIN_ENABLE_TRAINER)
    add_subdirectory(trainer)
endif(BRAIN_ENABLE_TRAINER)

if(BRAIN_ENABLE_DOC)
    find_package(Doxygen)

    if(DOXYGEN_FOUND)
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
        add_custom_target(doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
                          WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                          COMMENT "Generating API documentation with Doxygen" VERBATIM)
    endif(DOXYGEN_FOUND)

    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs/
            DESTINATION ${CMAKE_INSTALL_PREFIX}/docs)
endif(BRAIN_ENABLE_DOC)
