cmake_minimum_required(VERSION 2.8.9)
project(Brain)
set(CMAKE_BUILD_TYPE debug)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

find_package(LIBXML2)

set(SOURCES brain.c
            brain_descriptor.c
            brain_descriptor.h
            brain.h
            brain_type.h)

#Generate the shared library from the sources
add_library(Brain SHARED ${SOURCES})

target_link_libraries(Brain "-lm")

if (LIBXML2_FOUND)
    include_directories(${LIBXML2_INCLUDE_DIR})
    target_link_libraries(Brain ${LIBXML2_LIBRARIES})
endif()

# add a .pc file
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/brain.pc.in
               ${CMAKE_CURRENT_BINARY_DIR}/brain.pc @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/brain.pc
        DESTINATION lib/pkgconfig/)
