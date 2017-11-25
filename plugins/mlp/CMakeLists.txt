cmake_minimum_required(VERSION 2.8.9)
project(BrainMLP)

set(CMAKE_C_FLAGS
    ${CMAKE_C_FLAGS}
    "-O2 -g3 -Wall -Wextra -Wshadow -Wunused -Wunused-parameter")

if (NOT CMAKE_ENABLE_TESTING)
    set(CMAKE_C_VISIBILITY_PRESET hidden)
    set(CMAKE_CXX_VISIBILITY_PRESET hidden)
endif(NOT CMAKE_ENABLE_TESTING)

find_package(LIBXML2 REQUIRED)

configure_file ("${PROJECT_SOURCE_DIR}/include/brain_config.h.in"
                "${PROJECT_BINARY_DIR}/include/brain_config.h")

set(LIBBRAINMLP_SOURCE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(LIBBRAINMLP_INCLUDE_DIRS
    ${LIBXML2_INCLUDE_DIR}
    ${PROJECT_BINARY_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include)

include_directories(${LIBBRAINMLP_INCLUDE_DIRS})

file(GLOB_RECURSE SOURCES ${LIBBRAINMLP_SOURCE_DIR}/*.c)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

#Generate the shared library from the sources
add_library(BrainMLP SHARED ${SOURCES} ${HEADERS})

target_link_libraries(BrainMLP ${LIBXML2_LIBRARIES} BrainCore "-lm")
target_include_directories(BrainMLP PUBLIC ${LIBBRAINMLP_INCLUDE_DIRS})

install(TARGETS BrainMLP
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        COMPONENT all)

install(FILES ${HEADERS}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include
        COMPONENT headers)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/mlp.xml
        DESTINATION ${CMAKE_INSTALL_PREFIX}/api
        COMPONENT api)

install(DIRECTORY   ${CMAKE_CURRENT_SOURCE_DIR}/schemas/
        DESTINATION ${CMAKE_INSTALL_PREFIX}/schemas
        FILES_MATCHING PATTERN "*.xsd"  PATTERN "*.xsd")
