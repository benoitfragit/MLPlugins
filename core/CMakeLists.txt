cmake_minimum_required(VERSION 2.8.9)
project(BrainCore)

set(CMAKE_C_FLAGS
    ${CMAKE_C_FLAGS}
    "-o2 -g3 -Wall -Wextra -Wshadow -Wunused -Wunused-parameter")

find_package(LIBXML2 REQUIRED)

if (LIBBRAIN_DOUBLE_PRECISION)
    add_definitions(-DBRAIN_ENABLE_DOUBLE_PRECISION=1)
endif(LIBBRAIN_DOUBLE_PRECISION)

set(LIBBRAINCORE_SOURCE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(LIBBRAINCORE_INCLUDE_DIRS
    ${LIBXML2_INCLUDE_DIR}
    ${PROJECT_BINARY_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include)

include_directories(${LIBBRAINCORE_INCLUDE_DIRS})

file(GLOB_RECURSE SOURCES ${LIBBRAINCORE_SOURCE_DIR}/*.c)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

if (${CMAKE_ENABLE_LOGGING})
    add_definitions(-DBRAIN_ENABLE_LOGGING=1)
endif(${CMAKE_ENABLE_LOGGING})

#Generate the shared library from the sources
add_library(BrainCore SHARED ${SOURCES} ${HEADERS})

target_link_libraries(BrainCore ${LIBXML2_LIBRARIES} "-lm")
target_include_directories(BrainCore PUBLIC ${LIBBRAINCORE_INCLUDE_DIRS})

install(TARGETS BrainCore
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        COMPONENT all)

install(FILES ${HEADERS}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include
        COMPONENT headers)
