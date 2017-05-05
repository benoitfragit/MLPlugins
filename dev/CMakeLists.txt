cmake_minimum_required(VERSION 2.8.9)
project(Brain)

set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-o2 -g3 -Wall -Wextra -Wshadow -Wunused")

find_package(LIBXML2 REQUIRED)

configure_file ("${PROJECT_SOURCE_DIR}/src/brain_utils_config.h.in"
                "${PROJECT_BINARY_DIR}/src/brain_utils_config.h")

set(LIBBRAIN_SOURCE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(LIBBRAIN_INCLUDE_DIRS
    ${LIBXML2_INCLUDE_DIR}
    ${PROJECT_BINARY_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src)

include_directories(${LIBBRAIN_INCLUDE_DIRS})

file(GLOB_RECURSE SOURCES ${LIBBRAIN_SOURCE_DIR}/*.c)
file(GLOB_RECURSE HEADERS ${LIBBRAIN_SOURCE_DIR}/*.h)

#Generate the shared library from the sources
add_library(Brain SHARED ${SOURCES} ${HEADERS})

target_link_libraries(Brain ${LIBXML2_LIBRARIES} "-lm")

install(TARGETS Brain
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        COMPONENT all)

install(FILES ${HEADERS}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include
        COMPONENT headers)
