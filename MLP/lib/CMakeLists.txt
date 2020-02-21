cmake_minimum_required(VERSION 2.8.9)
project(MLP)

if (NOT BRAIN_ENABLE_TESTING)
    set(CMAKE_C_VISIBILITY_PRESET hidden)
    set(CMAKE_CXX_VISIBILITY_PRESET hidden)
endif(NOT BRAIN_ENABLE_TESTING)

foreach(SUBPATH ${CMAKE_PREFIX_PATH})
message (STATUS ">>>>>>>>>>>>>>" ${SUBPATH})
endforeach(SUBPATH)

configure_file ("${PROJECT_SOURCE_DIR}/include/mlp_config.h.in"
                "${PROJECT_BINARY_DIR}/include/mlp_config.h")

set(LIBBRAINMLP_SOURCE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(LIBBRAINMLP_INCLUDE_DIRS
    ${LIBXML2_INCLUDE_DIRS}
    ${PROJECT_BINARY_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include)

include_directories(${LIBBRAINMLP_INCLUDE_DIRS})

file(GLOB_RECURSE SOURCES ${LIBBRAINMLP_SOURCE_DIR}/*.c)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

#Generate the shared library from the sources
add_library(MLP SHARED ${SOURCES} ${HEADERS})

target_link_libraries(MLP PUBLIC ${LIBXML2_LIBRARIES} BrainCore)
target_include_directories(MLP PUBLIC ${LIBBRAINMLP_INCLUDE_DIRS})

install(TARGETS MLP
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
