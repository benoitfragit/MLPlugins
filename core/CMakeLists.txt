cmake_minimum_required(VERSION 2.8.9)
project(BrainCore)

set(LIBBRAINCORE_SOURCE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(LIBBRAINCORE_INCLUDE_DIRS
    ${LIBXML2_INCLUDE_DIRS}
    ${PROJECT_BINARY_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include)

include_directories(${LIBBRAINCORE_INCLUDE_DIRS})

file(GLOB_RECURSE SOURCES ${LIBBRAINCORE_SOURCE_DIR}/*.c)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

configure_file ("${PROJECT_SOURCE_DIR}/include/brain_core_config.h.in"
                "${PROJECT_BINARY_DIR}/include/brain_core_config.h")

#Generate the shared library from the sources
add_library(BrainCore STATIC ${SOURCES} ${HEADERS})

target_link_libraries(BrainCore PUBLIC ${LIBXML2_LIBRARIES})
target_include_directories(BrainCore PUBLIC ${LIBBRAINCORE_INCLUDE_DIRS})

install(TARGETS BrainCore
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        COMPONENT all)

install(FILES ${HEADERS}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include
        COMPONENT headers)

install(DIRECTORY   ${CMAKE_CURRENT_SOURCE_DIR}/schemas/
        DESTINATION ${CMAKE_INSTALL_PREFIX}/schemas
        FILES_MATCHING PATTERN "*.xsd"  PATTERN "*.xsd")
